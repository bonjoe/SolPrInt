/*
Copyright (c)   Written by Joevin Bonzi, University of Hohenheim, 440e
                bonzi.wiomoujoevin@uni-hohenheim.de
***************************************************************************/

#ifndef RLAGENTFUNCTION_H
#define RLAGENTFUNCTION_H

#include <math.h>    
#include <stdlib.h>  
#include <Arduino.h>

// Weights
extern double fc1_weights[32 * 7];
extern double fc1_biases[32];
extern double fc2_weights[32 * 32];
extern double fc2_biases[32];
extern double fc3_weights[10 * 32];
extern double fc3_biases[10];

void fullyConnectedLayer(double* inputNt, int input_size, double* outputNt, int output_size, double* weights, double* biases);
void relu(double* data, int size) ;
void forwardPass(double* input, double* output);
void chooseGreedyAction(double* input, int input_size, int& action_output);
void softmax(double* logits, double* probabilities);
#endif
