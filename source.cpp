// Created by Solomon on 1/31/2024.

#include "Circuit.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <regex>
#include <algorithm>
#include <random>
using namespace std;

ComplexNumber::ComplexNumber(double r, double i) : real(r), imag(i) {}

ComplexNumber::ComplexNumber() = default;

ComplexNumber ComplexNumber::operator+(const ComplexNumber& other) const {
    return {real + other.real, imag + other.imag};
}

ComplexNumber ComplexNumber::operator-(const ComplexNumber& other) const {
    return {real - other.real, imag - other.imag};
}

ComplexNumber ComplexNumber::operator*(const ComplexNumber& other) const {
    return {real * other.real - imag * other.imag, real * other.imag + imag * other.real};
}

ComplexNumber ComplexNumber::operator/(const ComplexNumber& other) const {
    double denom = other.real * other.real + other.imag * other.imag;
    return {(real * other.real + imag * other.imag) / denom, (imag * other.real - real * other.imag) / denom};
}

CircuitElement::CircuitElement(std::string  elem, int n1, int n2, double val)
        : element(std::move(elem)), firstNode(n1), secondNode(n2), value(val, 0.0) {}

void CircuitElement::showData() const {
    std::cout << element << "  " << firstNode << "  " << secondNode << "  " << value.real << "+" << value.imag << "i" << std::endl;
}

void CircuitElement::updateValue(double frequency) {
    std::regex pattern("L\\d+");
    if (std::regex_match(element, pattern)) {
        value.imag = 2 * M_PI * frequency * value.real;
        value.real = 0.0;
    }

    std::regex pattern2("C\\d+");
    if (std::regex_match(element, pattern2)) {
        value.imag = -1.0 / (2 * M_PI * frequency * value.real);
        value.real = 0.0;
    }
}

ComplexNumber CircuitElement::calculateParallelImpedance(const std::vector<CircuitElement>& elements, int leastRepeatedNode) {
    ComplexNumber totalAdmittance;

    for (const CircuitElement& element : elements) {
        if (element.firstNode == leastRepeatedNode || element.secondNode == leastRepeatedNode) {
            // Calculate admittance (1/impedance) for each parallel element
            // ComplexNumber admittance = {1.0 / element.value.real, -1.0 / element.value.imag};

            ComplexNumber admittance;

            if (element.value.imag == 0) {
                admittance = {(1 / element.value.real), 0.0}; // If imaginary part is 0, set the result's imaginary part to 0
            } else {
                admittance = {(element.value.real / ((element.value.real*element.value.real)+(element.value.imag*element.value.imag))), (-element.value.imag/ ((element.value.real*element.value.real)+(element.value.imag*element.value.imag)))};
            }


            if (element.value.real == 0) {
                admittance = {0.0, ( -element.value.imag/(element.value.imag*element.value.imag))};
            }

            totalAdmittance = totalAdmittance + admittance;

        }
    }

    // Calculate equivalent impedance using parallel connection
    double realPart;
    if (totalAdmittance.real == 0) {
        realPart = 0;
    } else {
        realPart = totalAdmittance.real /((totalAdmittance.real*totalAdmittance.real)+(totalAdmittance.imag*totalAdmittance.imag)) ;
    }
    double imagPart;
    if (totalAdmittance.imag == 0) {
        imagPart = 0;
    } else {
        imagPart = -totalAdmittance.imag /((totalAdmittance.real*totalAdmittance.real)+(totalAdmittance.imag*totalAdmittance.imag)) ;
    }
    return {realPart, imagPart};
}

void CircuitElement::test() const {
    double magnitude = sqrt(value.real * value.real + value.imag * value.imag);
    double phase = atan(value.imag/value.real);
    double phaseDegree = phase*(180/M_PI);
    cout << "|Z| =  " << magnitude << " ohm" << "   " << "arg(Z) = "<< phaseDegree << " degree"<< endl;
}

bool areNodesParallel(const CircuitElement& element1, const CircuitElement& element2) {
    return (element1.firstNode == element2.firstNode && element1.secondNode == element2.secondNode) ||
           (element1.firstNode == element2.secondNode && element1.secondNode == element2.firstNode);}

int findParallelNode(const std::vector<CircuitElement>& elements, int leastRepeatedNode) {
    for (size_t i = 0; i < elements.size(); ++i) {
        if (elements[i].firstNode == leastRepeatedNode || elements[i].secondNode == leastRepeatedNode) {
            for (size_t j = i + 1; j < elements.size(); ++j) {
                if (elements[j].firstNode == leastRepeatedNode || elements[j].secondNode == leastRepeatedNode) {
                    if (areNodesParallel(elements[i], elements[j])) {
                        return elements[i].firstNode == leastRepeatedNode ? elements[i].secondNode : elements[i].firstNode;
                    }
                }
            }
        }
    }
    return -1; // Indicates no parallel connection found
}

int findRandomRepeatedNode(const std::vector<CircuitElement>& elements, int previousNode) {
    std::unordered_map<int, int> nodeCounts;

    // Count occurrences of nodes in columns 2 and 3, excluding the 'previous' node
    for (const CircuitElement& element : elements) {
        if (element.firstNode != previousNode) {
            nodeCounts[element.firstNode]++;
        }
        if (element.secondNode != previousNode) {
            nodeCounts[element.secondNode]++;
        }
    }

    // Find nodes that appear more than once
    std::vector<int> repeatedNodes;
    for (const auto& entry : nodeCounts) {
        if (entry.second > 1) {
            repeatedNodes.push_back(entry.first);
        }
    }

    // If no nodes appear more than once, return a default value (you can change this as needed)
    if (repeatedNodes.empty()) {
        return -1;  // or any other value indicating no repeated nodes
    }

    // Choose a random repeated node
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(0, repeatedNodes.size() - 1);

    int randomIndex = distribution(generator);
    return repeatedNodes[randomIndex];
}


