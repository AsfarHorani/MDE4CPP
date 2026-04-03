#include <string>
#include <iostream>
#include <chrono>

#include "abstractDataTypes/SubsetUnion.hpp"
#include "ecore/ecoreFactory.hpp"
#include "ecore/ecorePackage.hpp"
#include "ecore/EAnnotation.hpp"
#include "ecore/EAttribute.hpp"
#include "ecore/EClass.hpp"
#include "ecore/EClassifier.hpp"
#include "ecore/EReference.hpp"
#include "ecore/EStringToStringMapEntry.hpp"
#include "ecore/EStructuralFeature.hpp"
#include "abstractDataTypes/Subset.hpp"

#define NUM_ELEMENT 1000000

using namespace ecore;

int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    {
		start = std::chrono::high_resolution_clock::now();
		//std::shared_ptr<ecorePackage> package=ecorePackage::eInstance();
		std::shared_ptr<ecoreFactory> factory = ecoreFactory::eInstance();
		end = std::chrono::high_resolution_clock::now();
		{
		std::shared_ptr<EPackage> package = factory->createEPackage();
		std::cout << "setup time: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

		// Benchmark section

		// Create Elements
		{
			start = std::chrono::high_resolution_clock::now();
			for (int var = 0; var < NUM_ELEMENT; ++var)
			{
				std::shared_ptr<EClass> c = factory->createEClass_as_eClassifiers_in_EPackage(package);
				c->setName(std::string("Class")+std::to_string(var));
				std::shared_ptr<EAttribute> a = factory->createEAttribute_as_eAttributes_in_EClass(c);
				a->setName(std::string("a") + std::to_string(var));
            }

			end = std::chrono::high_resolution_clock::now();
			std::cout << "time to create " << NUM_ELEMENT << " classes: " <<  std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
			std::shared_ptr<EAnnotation> a = factory->createEAnnotation_as_eAnnotations_in_EModelElement(package);


		}

		// Delete Elements
		start = std::chrono::high_resolution_clock::now();
		std::shared_ptr<Bag<ecore::EClassifier> > classifiers = package->getEClassifiers();
		std::shared_ptr<Subset<ecore::EClassifier, ecore::EObject>> subsetClassifiers;
		subsetClassifiers = std::dynamic_pointer_cast<Subset<ecore::EClassifier, ecore::EObject>>(classifiers);
		//for loop for every elements for delete in classifiers
		if(subsetClassifiers != nullptr)
		{
			std::cout << "Subset is not null" << std::endl;
			for (size_t i = 0; i < subsetClassifiers->size(); i++)
			{
				subsetClassifiers->erase(subsetClassifiers->at(0));
			}
			
		}


		end = std::chrono::high_resolution_clock::now();
		std::cout << "time to delete " << NUM_ELEMENT << " classes: " <<  std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
		std::shared_ptr< Bag<ecore::EObject> > contentUnions = package->getEContentUnion();
		std::cout<<classifiers->size()<<std::endl;
		std::cout<<contentUnions->size()<<std::endl;
		auto start2 = std::chrono::high_resolution_clock::now();
		contentUnions->clear();
		auto end2 = std::chrono::high_resolution_clock::now();
		std::cout << "time to delete UnionContent " << NUM_ELEMENT << " classes: " <<  std::chrono::duration_cast<std::chrono::milliseconds>(end2-start2).count() << std::endl;


		// Delete All
		start = std::chrono::high_resolution_clock::now();
	}
    }
	end = std::chrono::high_resolution_clock::now();

	std::cout << "time to delete package: " <<  std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
    return 0;
}
