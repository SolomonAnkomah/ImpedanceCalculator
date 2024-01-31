// Created by Solomon on 1/31/2024.

#ifndef CIRCUIT_H_INCLUDED
#define CIRCUIT_H_INCLUDED




#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502884L
#endif
using namespace std;




class ComplexNumber {
public:
    double real{};
    double imag{};

    ComplexNumber(double r, double i);
    ComplexNumber();

    ComplexNumber operator+(const ComplexNumber& other) const;
    ComplexNumber operator-(const ComplexNumber& other) const;
    ComplexNumber operator*(const ComplexNumber& other) const;
    ComplexNumber operator/(const ComplexNumber& other) const;
};

class CircuitElement {
public:
    std::string element;
    int firstNode;
    int secondNode;
    ComplexNumber value;

    CircuitElement(std::string  elem, int n1, int n2, double val);

    void showData() const;
    void updateValue(double frequency);
    static ComplexNumber calculateParallelImpedance(const std::vector<CircuitElement>& elements, int leastRepeatedNode);
    void test() const;


};

bool areNodesParallel(const CircuitElement& element1, const CircuitElement& element2);
int findParallelNode(const std::vector<CircuitElement>& elements, int leastRepeatedNode);
int findRandomRepeatedNode(const std::vector<CircuitElement>& elements, int previousNode);



#endif // CIRCUIT_H_INCLUDED
