#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "Z.h"
#include "Zmod.h"
#include <iostream>
#include <ostream>
#include <vector>

template <typename R> class Polynomial {
private:
  std::vector<R> coefficients;

public:
  Polynomial(const std::vector<R> &coefficients);
  int degree() const;
  std::vector<R> getCoefficients() const;
  R leadingCoefficient() const;
  Polynomial<R> operator+(const Polynomial<R> &other) const;
  Polynomial<R> operator-() const;
  Polynomial<R> operator*(const Polynomial<R> &other) const;
  std::pair<Polynomial<R>, Polynomial<R>>
  operator/(const Polynomial<R> &other) const;
  void printAsSequence() const;
  void printAsFunction() const;
};

// Implementierung hier, da Template-Klasse
// Konstruktor
template <typename R>
Polynomial<R>::Polynomial(const std::vector<R> &coefficients)
    : coefficients(coefficients) {}

template <typename R> int Polynomial<R>::degree() const {
  for (int i = coefficients.size() - 1; i >= 0; i--) {
    if (coefficients[i] != R(0))
      return i;
  }
  // mathematisch gesehen hat das Nullpolynom den Grad -1, hier aber 0 aus
  // technischen Gründen: (coefficients[-1] ist nicht definiert in C++)
  return 0;
}

template <typename R> std::vector<R> Polynomial<R>::getCoefficients() const {
  return coefficients;
}

template <typename R> R Polynomial<R>::leadingCoefficient() const {
  return coefficients[degree()];
}

template <typename R> void Polynomial<R>::printAsSequence() const {
  std::cout << "(";
  for (int i = 0; i < coefficients.size() - 1; i++) {
    std::cout << coefficients[i] << ", ";
  }
  std::cout << coefficients[coefficients.size() - 1];
  std::cout << ")" << std::endl;
}

template <typename R> void Polynomial<R>::printAsFunction() const {
  for (int i = degree(); i >= 0; i--) {
    if (coefficients[i] != R(0)) {
      // Vorzeichen für höchsten Grad setzen?
      if (i == degree()) {
        if (coefficients[i] < R(0)) {
          std::cout << "-";
        }
      } else {
        std::cout << (coefficients[i] < R(0) ? " - " : " + ");
      }

      // Koeffizienten nur anzeigen, wenn sie nicht 1 oder -1 sind oder bei
      // Konstante (i == 0)
      if (coefficients[i].abs() != R(1) || i == 0) {
        std::cout << coefficients[i].abs();
      }

      if (i > 0) {
        std::cout << "x";
        if (i > 1) {
          std::cout << "^" << i;
        }
      }
    }
  }
  std::cout << std::endl;
}

// Operationen auf Polynomen mit Koeffizienten aus dem Ring R
template <typename R>
Polynomial<R> Polynomial<R>::operator+(const Polynomial<R> &other) const {
  // dim(a+b) = max(dim(a), dim(b))
  int maxSize = std::max(coefficients.size(), other.coefficients.size());
  std::vector<R> resultCoefficients(maxSize);

  // "kürzeres" Polynom mit Nullen auffüllen, dann komponentenweise addieren
  for (int i = 0; i < maxSize; i++) {
    R coeff1 = (i < coefficients.size()) ? coefficients[i] : R(0);
    R coeff2 = (i < other.coefficients.size()) ? other.coefficients[i] : R(0);
    resultCoefficients[i] = coeff1 + coeff2;
  }
  return Polynomial(resultCoefficients);
}

template <typename R> Polynomial<R> Polynomial<R>::operator-() const {
  std::vector<R> negativeCoefficients(coefficients.size());

  for (int i = 0; i < negativeCoefficients.size(); i++) {
    negativeCoefficients[i] = -coefficients[i];
  }

  return Polynomial(negativeCoefficients);
}

template <typename R>
Polynomial<R> Polynomial<R>::operator*(const Polynomial<R> &other) const {
  // dim(a*b) = dim(a) + dim(b) = resultCoefficients.size() - 1
  std::vector<R> resultCoefficients((degree() + other.degree()) + 1);

  // Faltung der Koeffizienten, also Distributivgesetz aka Ausmultiplizieren
  for (int k = 0; k < resultCoefficients.size(); k++) {
    for (int i = 0; i <= k; i++) {
      // sicherstellen, dass hinter den Grenzen mit 0 multipliziert wird
      R c1 = (i < coefficients.size()) ? coefficients[i] : R(0);
      R c2 = ((k - i) < other.coefficients.size()) ? other.coefficients[k - i]
                                                   : R(0);
      resultCoefficients[k] = resultCoefficients[k] + c1 * c2;
    }
  }
  return Polynomial<R>(resultCoefficients);
}

template <typename R>
std::pair<Polynomial<R>, Polynomial<R>>
Polynomial<R>::operator/(const Polynomial<R> &other) const {
  if (other.degree() > degree()) {
    throw std::invalid_argument("Der Grad des Divisors darf nicht grösser sein "
                                "als der des Dividenten!");
  }
  if (!other.leadingCoefficient().isUnit()) {
    throw std::invalid_argument(
        "Der Leitkoeffizient des Divisors ist keine Einheit in " +
        std::string(typeid(R).name()));
  }
  Polynomial<R> r = *this;
  R u = other.leadingCoefficient().invert();
  std::vector<R> q(degree() - other.degree() + 1);
  for (int i = degree() - other.degree(); i >= 0; i--) {
    if (r.degree() == other.degree() + i) {
      q[i] = r.leadingCoefficient() * u;
      Polynomial<R> monom = Polynomial({q[i]});
      r = r + (-(monom * other));
    } else {
      q[i] = R(0);
    }
  }
  Polynomial<R> result = Polynomial(q);
  return std::make_pair(result, r);
}
#endif
