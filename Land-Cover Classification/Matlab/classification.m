%% Clear Window
clear all; close all; clc;

%% Read Images
load('D:\LandCoverImages\ground_truth.mat');
r_img = imread("D:\LandCoverImages\r.dib");
g_img = imread("D:\LandCoverImages\g.dib");
b_img = imread("D:\LandCoverImages\b.dib");
fe_img = imread("C:\LandCoverImages\fe.dib");
le_img = imread("C:\LandCoverImages\le.dib");
nir_img = imread("C:\LandCoverImages\nir.dib");
