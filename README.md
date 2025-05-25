# Matrix Multiplication using the Strassen-3 (Laderman) Algorithm in C++

## Overview

This project demonstrates an optimized algorithm for multiplying two 3×3 matrices using **Laderman's algorithm**, also known as the **Strassen-3** method. Unlike the classical algorithm which requires 27 scalar multiplications, Laderman's algorithm reduces this number to **23**, improving efficiency for fixed-size matrix multiplication.

## Features

- Matrix multiplication for 3×3 matrices using Laderman's algorithm
- Classical matrix multiplication for comparison
- Hybrid algorithm
- Matrix text files generator

## Cloning the Repository

This project uses Git submodules. After cloning repository, make sure to run:

```bash
git submodule update --init --recursive