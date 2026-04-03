#include <string>
#include <iostream>
#include <chrono>

#include "abstractDataTypes/SubsetUnion.hpp"
#include "ecore/ecoreFactory.hpp"
#include "ecore/impl/ecoreFactoryImpl.hpp"
#include "ecore/ecorePackage.hpp"
#include "ecore/impl/ecorePackageImpl.hpp"
#include "ecore/EAnnotation.hpp"
#include "ecore/EAttribute.hpp"
#include "ecore/EClass.hpp"
#include "ecore/EClassifier.hpp"
#include "ecore/EEnum.hpp"
#include "ecore/EOperation.hpp"
#include "ecore/EParameter.hpp"
#include "ecore/EReference.hpp"
#include "ecore/EStringToStringMapEntry.hpp"
#include "ecore/EStructuralFeature.hpp"


#define NUMBER_OF_OBJECTS 50000

using namespace ecore;

void pauseProgram() {
#ifdef _WIN32
    // If the OS is Windows, the compiler uses this:
    system("PAUSE");
#else
    // If the OS is Linux/Mac, the compiler uses this:
    std::cout << "Press Enter to continue..." << std::endl;
    // Clear any leftover newline characters in the input buffer
    if (std::cin.rdbuf()->in_avail() > 0) std::cin.ignore(256, '\n');
    std::cin.get();
#endif
}

int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    std::shared_ptr<ecoreFactory> factory = ecoreFactory::eInstance();

    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

    for (int var2 = 0; var2 < 5; ++var2)
    {
        std::cout<< "----------------------------   start next iteration  ---------------------------------------\n";
    	pauseProgram();
        start = std::chrono::high_resolution_clock::now();
        {
        	std::shared_ptr<EPackage> p = factory->createEPackage();
			std::shared_ptr<EEnum> e = factory->createEEnum_as_eClassifiers_in_EPackage(p);
			std::shared_ptr<EClass> t = factory->createEClass_as_eClassifiers_in_EPackage(p);
			t->setName("AClass");

			for (int var = 0; var<=NUMBER_OF_OBJECTS; ++var)
			{
				std::shared_ptr<EClass> c = factory->createEClass_as_eClassifiers_in_EPackage(p);
				std::shared_ptr<EAttribute> a = factory->createEAttribute_as_eAttributes_in_EClass(c);
				std::shared_ptr<EReference> r = factory->createEReference_as_eReferences_in_EClass(c);
				std::shared_ptr<EOperation> o = factory->createEOperation_as_eOperations_in_EClass(c);
				std::shared_ptr<EParameter> p = factory->createEParameter_as_eParameters_in_EOperation(o);

				c->setName("Class");
				e->setName("Enumeration");
				a->setName("Attribute");
				a->setEType(e);
				r->setName("Reference");
				r->setEType(t); // Use class c creates a ring dependency!
				o->setName("Operation");
				p->setName("Parameter");
				p->setEType(t);
			}

			end = std::chrono::high_resolution_clock::now();
			std::cout << "Time to create objects:" << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
pauseProgram();
            std::cout<< "----------- delete objects\n" << std::endl;
            start = std::chrono::high_resolution_clock::now();

            // --- SURGICAL FIX: FORCE DELETION ---
            // 1. Take a snapshot of the classifiers to avoid iterator invalidation
            auto classifiers = p->getEClassifiers();
            if (classifiers != nullptr) {
                std::vector<std::shared_ptr<ecore::EClassifier>> copyList;
                for(size_t i = 0; i < classifiers->size(); ++i) {
                    copyList.push_back(classifiers->at(i));
                }

                // 2. Command the package to erase them.
                // This manually triggers the Subset/Union update AND breaks the ring dependency!
				//size before
				std::cout << "Size before deletion: " << p->getEClassifiers()->size() << std::endl;
				for (const auto& element : copyList) {
                    p->getEClassifiers()->erase(element);
                }
            }

			std::cout << "Size after deletion: " << p->getEClassifiers()->size() << std::endl;
            // ------------------------------------

        } // <--- Now when it hits this bracket, the RAM will actually free!
        
        end = std::chrono::high_resolution_clock::now();

		std::cout << "Time to delete objects: " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() << std::endl;
    }
    std::cout<< "------------------------------- Finished  ------------------------------------\n";


	return 0;
}
