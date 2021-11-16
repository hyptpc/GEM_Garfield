#include <cstdlib>
#include <string>

#include "Garfield/SolidBox.hh"
#include "Garfield/GeometrySimple.hh"

#include "FieldMapBuilder.hpp"
#include "ParManager.hpp"
#include "Errors.hpp"

MediumMagboltz* FieldMapBuilder::initGas()
{
    auto parMan = ParManager::getInstance();
    if(fGas != nullptr)
        delete fGas;
    fGas = new MediumMagboltz();
    fGas->SetComposition(
        parMan->getParS("GAS_MEDIUM"), parMan->getParD("GAS_MEDIUM_FRAC"),
        parMan->getParS("GAS_QUENCHING"), parMan->getParD("GAS_QUENCHING_FRAC"));
    fGas->SetPressure(parMan->getParD("GAS_PRESSURE"));
    fGas->SetTemperature(parMan->getParD("GAS_TEMPERATURE"));
    fGas->Initialise(true);
    fGas->EnablePenningTransfer(parMan->getParD("GAS_PENNING_EFF"), parMan->getParD("GAS_PENNING_LAMB"),
        parMan->getParS("GAS_MEDIUM"));
    const std::string path = std::getenv("GARFIELD_INSTALL");
    fGas->LoadIonMobility(path + parMan->getParS("GAS_ION_MOBILITY"));
    fGas->SetMaxElectronEnergy(2000);
    return fGas;
}

ComponentConstant* FieldMapBuilder::buildDriftFieldMap()
{
    auto parMan = ParManager::getInstance();
    if(fGas == nullptr)
    {
        printError("FieldMapBuilder", "buildDriftFieldMap()", "fGas is a null pointer.");
        return nullptr;
    }
    const double tpcX = parMan->getParD("TPC_X");
    const double tpcY = parMan->getParD("TPC_Y");
    const double dZp = parMan->getParD("DZ_PADPLANE");
    const double dZ12 = parMan->getParD("DZ_GEM12");
    const double dZ23 = parMan->getParD("DZ_GEM23");
    const double dZu = parMan->getParD("DZ_UPPERPLANE");
    const double dZe = parMan->getParD("DZ_ELECTRODE");

    // define drift field region as simple box
    SolidBox *box = new SolidBox(-tpcX / 2, -tpcY / 2, dZp + dZ12 + dZ23 + dZu, tpcX, tpcY, dZe);
    GeometrySimple *geo = new GeometrySimple;
    geo->AddSolid(box, fGas);

    ComponentConstant* fm = new ComponentConstant();
    fm->SetMedium(fGas);
    fm->SetGeometry(geo);
    fm->SetElectricField(0., 0., parMan->getParD("E_DRIFT"));
    return fm;
}

ComponentElmer* FieldMapBuilder::buildGemFieldMap(const bool print)
{
    if(fGas == nullptr)
    {
        printError("FieldMapBuilder", "buildGemFieldMap(const bool)", "fGas is a null pointer.");
        return nullptr;
    }
    auto parMan = ParManager::getInstance();
    string script = parMan->getParS("SCRIPT_NAME");

    // get an absolute path to executable.
    char buffer[1024];
    ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer)/sizeof(char));
    buffer[length] = '\0';
    std::string path = buffer;
    path = path.substr(0, path.find_last_of('/') + 1) + script;
    std::cout << path << std::endl;
    ComponentElmer* fm = new ComponentElmer(
        path + "/mesh.header", path + "/mesh.elements", path + "/mesh.nodes",
        path + "/dielectrics.dat", path + "/" + script + ".result", "cm");
    fm->EnablePeriodicityX();
    fm->EnablePeriodicityY();
    // Associate the gas with the corresponding field map material.
    const unsigned int nMaterials = fm->GetNumberOfMaterials();
    for (unsigned int i = 0; i < nMaterials; ++i)
    {
        const double eps = fm->GetPermittivity(i);
        if (eps == 1.)
            fm->SetMedium(i, fGas);
    }
    fm->EnableConvergenceWarnings(false);
    fm->PrintMaterials();
    return fm;
}

ComponentConstant* FieldMapBuilder::buildMagneticField()
{
    auto parMan = ParManager::getInstance();
    if(fGas == nullptr)
    {
        printError("FieldMapBuilder", "buildMagneticField()", "fGas is a null pointer.");
        return nullptr;
    }
    const double tpcX = parMan->getParD("TPC_X");
    const double tpcY = parMan->getParD("TPC_Y");
    const double dZp = parMan->getParD("DZ_PADPLANE");
    const double dZ12 = parMan->getParD("DZ_GEM12");
    const double dZ23 = parMan->getParD("DZ_GEM23");
    const double dZu = parMan->getParD("DZ_UPPERPLANE");
    const double dZe = parMan->getParD("DZ_ELECTRODE");
    // define constant magnetic field region as simple box
    SolidBox *box = new SolidBox(-tpcX / 2, -tpcY / 2, 0., tpcX, tpcY, dZp + dZ12 + dZ23 + dZu + dZe);
    GeometrySimple *geo = new GeometrySimple;
    geo->AddSolid(box, fGas);

    ComponentConstant* fm = new ComponentConstant();
    fm->SetMedium(fGas);
    fm->SetGeometry(geo);
    fm->SetMagneticField(parMan->getParD("B_X"), parMan->getParD("B_Y"), parMan->getParD("B_Z"));
    return fm;
}