// =============================================================================
// PROJECT CHRONO - http://projectchrono.org
//
// Copyright (c) 2014 projectchrono.org
// All rights reserved.
//
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file at the top level of the distribution and at
// http://projectchrono.org/license-chrono.txt.
//
// =============================================================================
// Authors: Alessandro Tasora
// =============================================================================
//
// Demo code about
//     - collisions and contacts
//     - using Irrlicht to display objects.
//
// =============================================================================

#include "chrono/physics/ChSystemSMC.h"
#include "chrono/physics/ChBodyEasy.h"
#include "chrono/physics/ChParticlesClones.h"
#include "chrono/physics/ChLinkMotorRotationSpeed.h"
#include "chrono/assets/ChTexture.h"
#include "chrono_irrlicht/ChIrrApp.h"



// Use the namespaces of Chrono
using namespace chrono;
using namespace chrono::irrlicht;

// Use the main namespaces of Irrlicht
using namespace irr;
using namespace irr::core;
using namespace irr::scene;
using namespace irr::video;
using namespace irr::io;
using namespace irr::gui;
using namespace std;

void create_bead(double r_bead, ChSystemSMC& mphysicalSystem, ChVector<> pos, double mass, bool isFixed, bool isWall, std::vector< std::shared_ptr< ChBody > > list) {
	ChQuaternion<> rot(1, 0, 0, 0);
	ChVector<> init_vel(0, 0, 0);
	
	auto material = std::make_shared<ChMaterialSurfaceSMC>();
	material->SetRestitution(0.1f);
	material->SetFriction(0.4f);
	material->SetAdhesion(0);

	auto body = std::make_shared<ChBody>(ChMaterialSurface::SMC);
	
	body->SetMass(mass);
	body->SetPos(pos);
	body->SetRot(rot);
	body->SetPos_dt(init_vel);
	if (isFixed == true) {
		body->SetBodyFixed(true);
	}
	else {
		body->SetBodyFixed(false);
	}
	body->SetMaterialSurface(material);
	body->SetCollide(true);


	body->GetCollisionModel()->ClearModel();
	body->GetCollisionModel()->AddSphere(r_bead);
	body->GetCollisionModel()->BuildModel();
	
	body->SetInertiaXX(0.4 * mass * r_bead * r_bead * ChVector<>(1, 1, 1));
	auto sphere = std::make_shared<ChSphereShape>();
	sphere->GetSphereGeometry().rad = r_bead;
	sphere->SetColor(ChColor(0.9f, 0.4f, 0.2f));
	body->AddAsset(sphere);

	
	auto text = std::make_shared<ChTexture>();
	if (isWall == true) {
		text->SetTextureFilename(GetChronoDataFile("greenwhite.png"));
		
		list.push_back(body);
	}
	else {
		text->SetTextureFilename(GetChronoDataFile("bluwhite.png"));
	}
	body->AddAsset(text);

	mphysicalSystem.AddBody(body);
}

void create_cylinder_ext(ChSystemSMC& mphysicalSystem, ISceneManager* msceneManager, IVideoDriver* driver, double r_bead, double r_cyl_ext,double height, int methode, double mass, std::vector< std::shared_ptr< ChBody > > list) {
	
	if (methode == 1) { //Remplissage en colonne
		
		for (int i = 0; i < floor(PI*(r_cyl_ext-r_bead) / r_bead) ; i++) {
			for (int j = 0; j < floor(height / (2 * r_bead)); j = j++) {
				ChVector <> pos = ChVector<>((r_cyl_ext - r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))), r_bead * 2 * j + r_bead, (r_cyl_ext - r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass,true,true, list);
			}
		}
	}

	else if (methode == 2) { //Remplissage en décalé sans contact vertical
		for (int j = 0; j < floor(height / (2 * r_bead)); j = j + 2) {
			for (int i = 0; i < floor((PI*(r_cyl_ext-r_bead)) / r_bead)+1; i++) {
				ChVector<> pos = ChVector<>((r_cyl_ext - r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))), r_bead * 2 * j + r_bead, (r_cyl_ext - r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass,true,true, list);

				ChVector<> pos2= ChVector<>((r_cyl_ext - r_bead)*cos((2 * i + 1)*(atan(r_bead / (r_cyl_ext - r_bead)))), r_bead * 2 * (j + 1) + r_bead, (r_cyl_ext - r_bead)*sin((2 * i + 1)*(atan(r_bead / (r_cyl_ext - r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos2, mass,true,true,list);
			}
		}
	}

	else if (methode == 3) { //Remplissage en décalé avec contact vertical
		for (int j = 0; j < floor(height / (2 * r_bead)); j = j + 2) {
			for (int i = 0; i < floor((PI*(r_cyl_ext-r_bead)) / r_bead)+1  ; i++) {
				ChVector<> pos= ChVector<>((r_cyl_ext - r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))), sqrt(3)*r_bead * j + r_bead, (r_cyl_ext - r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_ext - r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass,true,true,list);

				ChVector<> pos2 = ChVector<>((r_cyl_ext - r_bead)*cos((2 * i + 1)*(atan(r_bead / (r_cyl_ext - r_bead)))), sqrt(3)*r_bead * (j + 1) + r_bead, (r_cyl_ext - r_bead)*sin((2 * i + 1)*(atan(r_bead / (r_cyl_ext - r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos2, mass,true, true,list);
			}
		}
	}

	else {
		printf("La methode rentree est incorrecte\n");
	}

}

void create_cylinder_int(ChSystemSMC& mphysicalSystem, ISceneManager* msceneManager, IVideoDriver* driver, double r_bead, double r_cyl_int, double height, int methode, std::shared_ptr<chrono::ChBodyFrame> rotatingBody, double mass, std::vector< std::shared_ptr< ChBody > > list) {
	

	
	if (methode == 1) { //Remplissage en colonne
		for (int i = 0; i < floor((PI*(r_cyl_int+r_bead)) / r_bead) ; i++) {
			for (int j = 0; j < floor(height / (2 * r_bead)); j++) {
				ChVector<> pos = ChVector<>((r_cyl_int + r_bead)*cos(i*(2 * atan(r_bead / r_cyl_int))), r_bead * 2 * j + r_bead, (r_cyl_int + r_bead)*sin(i*(2 * atan(r_bead / r_cyl_int))));
				create_bead(r_bead, mphysicalSystem, pos, mass,false, true, list);
				
				auto lock = std::make_shared<ChLinkMateFix>();
				lock->Initialize(mphysicalSystem.Get_bodylist().back(), rotatingBody);
				mphysicalSystem.AddLink(lock);
			}
		}
	}

	else if (methode == 2) { //Remplissage en décalé sans contact vertical
		for (int j = 0; j < floor(height / (2 * r_bead)); j = j + 2) {
			for (int i = 0; i < floor((PI*(r_cyl_int+r_bead)) / r_bead) +1; i++) {
				ChVector<> pos = ChVector<>((r_cyl_int + r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_int + r_bead)))), r_bead * 2 * j + r_bead, (r_cyl_int + r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_int + r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass,false,true, list);
				
				auto lock = std::make_shared<ChLinkMateFix>();
				lock->Initialize(mphysicalSystem.Get_bodylist().back(), rotatingBody);
				mphysicalSystem.AddLink(lock);

				ChVector<> pos2 = ChVector<>((r_cyl_int + r_bead)*cos((2 * i + 1)*(atan(r_bead / (r_cyl_int + r_bead)))), r_bead * 2 * (j + 1) + r_bead, (r_cyl_int + r_bead)*sin((2 * i + 1)*(atan(r_bead / (r_cyl_int + r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos2, mass,false,true, list);

				auto lock2 = std::make_shared<ChLinkMateFix>();
				lock2->Initialize(mphysicalSystem.Get_bodylist().back(), rotatingBody);
				mphysicalSystem.AddLink(lock2);
			}
		}
	}

	else if (methode == 3) { //Remplissage en décalé avec contact vertical
		for (int j = 0; j < floor(height / (2 * r_bead)); j = j + 2) {
			for (int i = 0; i < floor( PI*(r_cyl_int+r_bead) / r_bead) +1; i++) {

				ChVector<> pos = ChVector<>((r_cyl_int + r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_int + r_bead)))), sqrt(3)*r_bead * j + r_bead, (r_cyl_int + r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_int + r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass,false,true, list);

				auto lock = std::make_shared<ChLinkMateFix>();
				lock->Initialize(mphysicalSystem.Get_bodylist().back(), rotatingBody);
				mphysicalSystem.AddLink(lock);

				ChVector<> pos2 = ChVector<>((r_cyl_int + r_bead)*cos((2 * i + 1)*(atan(r_bead / (r_cyl_int + r_bead)))), sqrt(3)*r_bead * (j + 1) + r_bead, (r_cyl_int + r_bead)*sin((2 * i + 1)*(atan(r_bead / (r_cyl_int + r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos2, mass,false,true, list);

				auto lock2 = std::make_shared<ChLinkMateFix>();
				lock2->Initialize(mphysicalSystem.Get_bodylist().back(), rotatingBody);
				mphysicalSystem.AddLink(lock2);
			}
		}
	}

	else {
		printf("La methode rentree est incorrecte\n");
	}
}

void remplir(ChSystemSMC& mphysicalSystem, ISceneManager* msceneManager, IVideoDriver* driver, double r_bead, double r_cyl_int, double r_cyl_ext, double mass, int methode, double height_bead, std::vector< std::shared_ptr< ChBody > > list) {
	if (methode == 1) {
		for (int k = 0; k < floor(((r_cyl_ext-r_bead) - (r_cyl_int+r_bead)) / (2*r_bead))-1; k++) {
			for (int j = 0; j < floor(height_bead / (2 * r_bead)); j = j + 2) {
				for (int i = 0; i < floor((PI*(r_cyl_int + 3 * r_bead + 2 * k*r_bead)) / r_bead) ; i++) {
					ChVector <> pos = ChVector<>((r_cyl_int+3*r_bead+2*k*r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_int + 3 * r_bead + 2 * k*r_bead)))), r_bead  * j + r_bead, (r_cyl_int + 3 * r_bead + 2 * k*r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_int + 3 * r_bead + 2 * k*r_bead)))));
					create_bead(r_bead, mphysicalSystem, pos, mass, false, false, list);
				}
			}
		}
	}

	 else if (methode == 2) {
		for (int k = 0; k < floor(((r_cyl_ext - r_bead) - (r_cyl_int + r_bead)) / (2 * r_bead)) - 1; k++) {
			for (int i = 0; i < floor((PI*(r_cyl_int + 3 * r_bead + 2 * k*r_bead)) / r_bead); i++) {
				ChVector <> pos = ChVector<>((r_cyl_int + 3 * r_bead + 2 * k*r_bead)*cos(i*(2 * atan(r_bead / (r_cyl_int + 3 * r_bead + 2 * k*r_bead)))), 2*height_bead, (r_cyl_int + 3 * r_bead + 2 * k*r_bead)*sin(i*(2 * atan(r_bead / (r_cyl_int + 3 * r_bead + 2 * k*r_bead)))));
				create_bead(r_bead, mphysicalSystem, pos, mass, false, false, list);
			}
		}
	}

	 else {
		printf("La methode demandee est incorrecte");
	}
}

std::vector <std::vector< std::shared_ptr< ChBody > >> create_some_falling_items(ChSystemSMC& mphysicalSystem, ISceneManager* msceneManager, IVideoDriver* driver,double r_cyl_int, double r_cyl_ext, double height, double r_bead, double mass, double height_bead) {
	
	//Création du sol
	auto material = std::make_shared<ChMaterialSurfaceSMC>();
	material->SetRestitution(0.1f);
	material->SetFriction(0.4f);
	material->SetAdhesion(0);

	auto fixedBody = std::make_shared<ChBody>(ChMaterialSurface::SMC);

	fixedBody->SetMass(1.0);
	fixedBody->SetBodyFixed(true);
	fixedBody->SetPos(ChVector<>(0,-0.5,0));
	fixedBody->SetCollide(true);
	fixedBody->SetMaterialSurface(material);
	fixedBody->GetCollisionModel()->ClearModel();
	fixedBody->GetCollisionModel()->AddBox(r_cyl_ext, 1,r_cyl_ext,fixedBody->GetPos(), fixedBody->GetRot() );
	fixedBody->GetCollisionModel()->BuildModel();

	auto cylinder = std::make_shared<ChBoxShape>();
	cylinder->GetBoxGeometry().Size = ChVector<>(r_cyl_ext,0.5, r_cyl_ext);
	cylinder->GetBoxGeometry().Pos = ChVector<>(0,0,0);
	cylinder->SetColor(ChColor(1, 0, 1));
	cylinder->SetFading(0.6f);
	fixedBody->AddAsset(cylinder);

	auto textcyl = std::make_shared<ChTexture>();
	textcyl->SetTextureFilename(GetChronoDataFile("blu.png"));
	fixedBody->AddAsset(textcyl);

	mphysicalSystem.AddBody(fixedBody);
	
	// Add the rotating mixer

	auto rotatingBody = std::make_shared<ChBody>(ChMaterialSurface::SMC);

	rotatingBody->SetMass(10.0);
	rotatingBody->SetInertiaXX(ChVector<>(50, 50, 50));
	rotatingBody->SetPos(ChVector<>(0, -1, 0));
	rotatingBody->SetCollide(true);
	rotatingBody->SetMaterialSurface(material);

	rotatingBody->GetCollisionModel()->ClearModel();
	rotatingBody->GetCollisionModel()->AddCylinder(r_cyl_int, r_cyl_int, height - 1, rotatingBody->GetPos(), rotatingBody->GetRot());
	rotatingBody->GetCollisionModel()->BuildModel();

	auto box = std::make_shared<ChCylinderShape>();
	box->GetCylinderGeometry().rad = r_cyl_int;
	box->GetCylinderGeometry().p1 = ChVector<>(0, height-1, 0);
	box->GetCylinderGeometry().p2 = ChVector<>(0, 0, 0);
	box->SetColor(ChColor(0, 0, 1));
	box->SetFading(0.6f);
	rotatingBody->AddAsset(box);

	mphysicalSystem.AddBody(rotatingBody);

	
	// .. a motor between mixer and truss

	auto my_motor = std::make_shared<ChLinkMotorRotationSpeed>();
	my_motor->Initialize(rotatingBody, fixedBody, ChFrame<>(ChVector<>(0, 0, 0), Q_from_AngAxis(CH_C_PI_2, VECT_X)));
	auto mfun = std::make_shared<ChFunction_Const>(CH_C_PI / 2.0);  // speed w=90°/s
	//auto mfun = std::make_shared<ChFunction_Const>(0);
	my_motor->SetSpeedFunction(mfun);
	mphysicalSystem.AddLink(my_motor);

	// optional, attach a texture for better visualization
	auto mtexture = std::make_shared<ChTexture>();
	mtexture->SetTextureFilename(GetChronoDataFile("bluwhite.png"));
	rotatingBody->AddAsset(mtexture);
	

	std::vector< std::shared_ptr< ChBody > > cylinder_int_list;
	create_cylinder_ext(mphysicalSystem, msceneManager, driver, r_bead, r_cyl_ext, height, 3, mass, cylinder_int_list);

	std::vector< std::shared_ptr< ChBody > > cylinder_ext_list;
	create_cylinder_int(mphysicalSystem, msceneManager, driver, r_bead, r_cyl_int, height, 3, rotatingBody, mass, cylinder_ext_list);
	
	std::vector< std::shared_ptr< ChBody > > beads_list;
	remplir(mphysicalSystem, msceneManager, driver, r_bead, r_cyl_int, r_cyl_ext, mass, 1, height_bead, beads_list);

	std::vector <std::vector< std::shared_ptr< ChBody > >> list_list;
	list_list.push_back(cylinder_ext_list);
	list_list.push_back(cylinder_int_list);
	list_list.push_back(beads_list);

	return list_list;
}

int main(int argc, char* argv[]) {
	GetLog() << "Copyright (c) 2017 projectchrono.org\nChrono version: " << CHRONO_VERSION << "\n\n";

	//Déclaration des paramètres
	double gravity = -9.81;
	double r_bead = 0.5;
	double r_cyl_ext =10 ;
	double r_cyl_int = 4;
	double height = 10;
	double height_bead = 10;
	double mass = 1;

	//Paramètres de simulation
	double time_step = 1e-4;//1e-4
	double out_step = 0.02;
	double time = 0;
	double out_time = 0;

	// Create a ChronoENGINE physical system
	ChSystemSMC mphysicalSystem;
	mphysicalSystem.SetContactForceModel(ChSystemSMC::ContactForceModel::Hertz);
	mphysicalSystem.SetAdhesionForceModel(ChSystemSMC::AdhesionForceModel::Constant);
	mphysicalSystem.Set_G_acc(ChVector<>(0, gravity, 0));

	// Create the Irrlicht visualization (open the Irrlicht device,
	// bind a simple user interface, etc. etc.)
	ChIrrApp application(&mphysicalSystem, L"Collisions between objects", core::dimension2d<u32>(800, 600), false, true);
	/*ChIrrApp application(&mphysicalSystem, L"ChLinkLockPlanePlane", irr::core::dimension2d<irr::u32>(800, 600), false, true);*/

	// Easy shortcuts to add camera, lights, logo and sky in Irrlicht scene:
	ChIrrWizard::add_typical_Logo(application.GetDevice());
	ChIrrWizard::add_typical_Sky(application.GetDevice());
	ChIrrWizard::add_typical_Lights(application.GetDevice());
	ChIrrWizard::add_typical_Camera(application.GetDevice(), core::vector3df(0, 30, 0));
	/*application.AddTypicalCamera(irr::core::vector3df(300, 0, 300));*/
	
	

	// Create all the rigid bodies.

	std::vector <std::vector< std::shared_ptr< ChBody > >> list_list;
	list_list = create_some_falling_items(mphysicalSystem, application.GetSceneManager(), application.GetVideoDriver(),r_cyl_int,r_cyl_ext,height,r_bead, mass, height_bead);
	
	const std::vector< std::shared_ptr< ChBody > > list = list_list.back();
	const char* name = list[list.size()-1]->GetName();
	printf("Le nom de l'element est %s\n",name);
	ChVector< double > pos = list.back()->GetPos();
	printf("coucou");
	printf("La position est (%f,%f,%f) \n", list.back()->GetPos().x(), list.back()->GetPos().y(), list.back()->GetPos().z());

	collision::ChCollisionInfo::SetDefaultEffectiveCurvatureRadius(r_bead);

	// Use this function for adding a ChIrrNodeAsset to all items
	// Otherwise use application.AssetBind(myitem); on a per-item basis.
	application.AssetBindAll();

	// Use this function for 'converting' assets into Irrlicht meshes
	application.AssetUpdateAll();

	
	

	// mphysicalSystem.SetUseSleeping(true);

	application.SetStepManage(true);
	application.SetTimestep(0.02);

	//
	// THE SOFT-REAL-TIME CYCLE
	
	while (application.GetDevice()->run()) {
		application.BeginScene();
		ChIrrTools::drawSegment(application.GetVideoDriver(), ChVector<>(0, -100, 0), ChVector<>(0, 100, 0), irr::video::SColor(255, 0, 0, 0), true);
		ChIrrTools::drawGrid(application.GetVideoDriver(), 0.2, 0.2, 20, 20,
			ChCoordsys<>(ChVector<>(0, 0, 0), Q_from_AngX(CH_C_PI_2)),
			video::SColor(255, 80, 100, 100), true);
		application.DrawAll();
		//printf("La position est (%f,%f,%f) \n", list.back()->GetPos().x(), list.back()->GetPos().y(), list.back()->GetPos().z());
		while (time < out_time) {
			mphysicalSystem.DoStepDynamics(time_step);
			time += time_step;
		}
		out_time += out_step;

		application.EndScene();
	}
	

	return 0;
}