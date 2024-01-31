#include "Circuit.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;
string fileName;

int main() {
    cout << "Enter file name (include file extension .txt): ";
    cin >> fileName;

    ifstream inputFile(fileName);

    if (!inputFile.is_open()) {
        cerr << "Error opening the file!" << std::endl;
        return 1;
    }

    ComplexNumber one = {1,0};

    vector<CircuitElement> mesh;
    vector<CircuitElement> parallel;
    vector<CircuitElement> elements;
    string elem;
    int n1, n2;
    double val;

    double frequency = 0.0;  // Variable to store frequency from P element
    int pNode1,pNode2;

    while (inputFile >> elem) {
        // Check if the line starts with '#'
        if (elem[0] == '#') {
            // Skip the rest of the line
            inputFile.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (elem == "P") {
            inputFile >> n1 >> n2 >> val;
            frequency = val;
            pNode1 = n1;
            pNode2 = n2;
        } else if (elem == "#") {
            continue; // Ignore the whole line
        } else {
            inputFile >> n1 >> n2 >> val;
            CircuitElement circuitElement(elem, n1, n2, val);
            elements.push_back(circuitElement);
        }
    }


    inputFile.close();

    // Update values for L and C elements based on the frequency
    for (CircuitElement& element : elements) {
        element.updateValue(frequency); // Update values for L and C elements
    }
    int previous = -1;

    while (elements.size() > 1) {
        // Find the least repeated node

        int leastRepeatedNode = findRandomRepeatedNode(elements,previous);


        // Display the element connected to the least repeated node
        cout << "The least repeated node (greater than 1) is: " << leastRepeatedNode << endl;
        cout << "Element connected to the least repeated node:" << endl;

        for (const CircuitElement &element: elements) {
            if (element.firstNode == leastRepeatedNode || element.secondNode == leastRepeatedNode) {
                element.showData();
                mesh.push_back(element);
            }
        }

        // Determine the type of connection and calculate resultant impedance
        int parallelNode = findParallelNode(mesh, leastRepeatedNode);

        if (parallelNode != -1) {
            for (const CircuitElement &element: mesh) {
                if (element.firstNode == parallelNode || element.secondNode == parallelNode) {
                    parallel.push_back(element);
                }
            }

            cout << "Parallel Connection Detected" << endl;
            int i = 0;


            // Calculate parallel impedance and store as new element
            CircuitElement parallelElement("Z", parallel[i].firstNode, parallel[i].secondNode, 0.0);
            parallelElement.value = parallel[i].calculateParallelImpedance(parallel, parallelNode);



            // Display the resultant impedance
            cout << "Resultant Impedance (Z" << elements.size() << "): ";
            parallelElement.showData();

            parallel.erase(remove_if(parallel.begin(), parallel.end(),
                                     [parallelNode](const CircuitElement &elem) {
                                         return elem.firstNode == parallelNode ||
                                                elem.secondNode == parallelNode;
                                     }), parallel.end());

            mesh.erase(remove_if(mesh.begin(), mesh.end(),
                                 [parallelNode](const CircuitElement &elem) {
                                     return elem.firstNode == parallelNode ||
                                            elem.secondNode == parallelNode;
                                 }), mesh.end());

            // Remove all elements involved in the parallel calculation
            elements.erase(remove_if(elements.begin(), elements.end(),
                                     [leastRepeatedNode](const CircuitElement &elem) {
                                         return elem.firstNode == leastRepeatedNode ||
                                                elem.secondNode == leastRepeatedNode;
                                     }), elements.end());

            for (const CircuitElement &remainingElement : parallel) {
                elements.push_back(remainingElement);
            }

            for (const CircuitElement &remainingElement : mesh) {
                elements.push_back(remainingElement);
            }

            // Add the new parallelElement to the elements vector
            elements.push_back(parallelElement);

            parallel.clear();

            mesh.clear();

            //previous = leastRepeatedNode;


        } else {

            if(leastRepeatedNode == pNode1 || leastRepeatedNode == pNode2){


                previous = leastRepeatedNode == pNode2 ? pNode2 : pNode1 ;
                parallel.clear();

                mesh.clear();



            }else {
                ComplexNumber ConductanceSum;


                for (const CircuitElement &element: elements) {
                    if (element.firstNode == leastRepeatedNode || element.secondNode == leastRepeatedNode) {
                        ComplexNumber series = one / element.value;
                        ConductanceSum = ConductanceSum + series;
                    }
                }
                ComplexNumber equivalentConductance = {ConductanceSum.real, ConductanceSum.imag};


                for (int i = 0; i < mesh.size(); ++i) {
                    for (int n = i + 1; n < mesh.size(); ++n) {
                        ComplexNumber conductanceI = one / mesh[i].value;
                        ComplexNumber conductanceN = one / mesh[n].value;

                        ComplexNumber NewConductance = (conductanceI * conductanceN) / equivalentConductance;
                        ComplexNumber ImpedanceNew = one / NewConductance;

                        int otherNode1, otherNode2;

                        if (mesh[i].firstNode == leastRepeatedNode) {
                            otherNode1 = mesh[i].secondNode;
                        } else {
                            otherNode1 = mesh[i].firstNode;
                        }
                        if (mesh[n].firstNode == leastRepeatedNode) {
                            otherNode2 = mesh[n].secondNode;
                        } else {
                            otherNode2 = mesh[n].firstNode;
                        }

                        CircuitElement resultantElement("Z", otherNode1, otherNode2, 0.0);
                        resultantElement.value = ImpedanceNew;

                        cout << "Resultant Element (Z" << elements.size() << "): ";
                        resultantElement.showData();

                        elements.push_back(resultantElement);
                    }
                }

                elements.erase(remove_if(elements.begin(), elements.end(),
                                         [leastRepeatedNode](const CircuitElement &elem) {
                                             return elem.firstNode == leastRepeatedNode ||
                                                    elem.secondNode == leastRepeatedNode;

                                         }), elements.end());

                mesh.clear();

                //previous = leastRepeatedNode;

            }

        }

        // Store the elements connected to the least repeated node in the 'mesh' vector

    }


    cout << "\nFinal Circuit Elements after Computations:" << endl;
    for (const CircuitElement& element : elements) {
        element.showData();
        cout << "\n" << endl;

        cout << "********************************************************************" << endl;
        cout << "Z = ( " << element.value.real << " + " << "j*" <<  element.value.imag << " ) ohm " << "\n" << endl;

        element.test();

        cout << "********************************************************************" << endl;
    }


    return 0;
}
