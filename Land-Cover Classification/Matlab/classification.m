%% Clear Window
clear all; close all; clc;

%% Read Images
load('C:\LandCoverImages\ground_truth.mat');
r_img = imread("C:\LandCoverImages\r.dib");
g_img = imread("C:\LandCoverImages\g.dib");
b_img = imread("C:\LandCoverImages\b.dib");
fe_img = imread("C:\LandCoverImages\fe.dib");
le_img = imread("C:\LandCoverImages\le.dib");
nir_img = imread("C:\LandCoverImages\nir.dib");

%% coordinates for each class
stats = regionprops(labelled_ground_truth, 'PixelList');

%% Initialize Variables

% empty array
r_arr = ([]);
g_arr = ([]);
b_arr = ([]);
fe_arr =([]);
le_arr = ([]);
nir_arr = ([]);

    
% empty cell
SampleCell = {};
covCell = {};
    
% loop for 4 classes
for k = 1:size(stats,1)
    
    % loop for each co-ordinates, per class from each images
    for i = 1:size(stats(k).PixelList,1)
        r_arr(i,1) = r_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        g_arr(i,1) = g_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        b_arr(i,1) = b_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        fe_arr(i,1) = fe_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        le_arr(i,1) = le_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
        nir_arr(i,1) = nir_img(stats(k).PixelList(i,2), stats(k).PixelList(i,1));
    end
    
    % 20 samples per image 
    rng(101);
    pixelSize = 50; % percentage
    SampleCell(:,1,k) = {datasample(r_arr,pixelSize,'Replace',false)}; 
    SampleCell(:,2,k) = {datasample(g_arr,pixelSize,'Replace',false)};
    SampleCell(:,3,k) = {datasample(b_arr,pixelSize,'Replace',false)};
    SampleCell(:,4,k) = {datasample(fe_arr,pixelSize,'Replace',false)};
    SampleCell(:,5,k) = {datasample(le_arr,pixelSize,'Replace',false)};
    SampleCell(:,6,k) = {datasample(nir_arr,pixelSize,'Replace',false)};

    
    % mean for each image per class, total 6 means per class
    muArray(:,k) = transpose(mean(cell2mat(SampleCell(:,:,k)),1));

    % covariance matrix is 6X6 for each class
    covCell(k,:) = {cov(cell2mat(SampleCell(:,:,k)))};

 end
 
 
 %% Testing
    % create result matrix
    % predicted = None(size(labelled_ground_truth,1),size(labelled_ground_truth,2));
    predicted = ([]);
    % Test
    fori = 1:size(labelled_ground_truth,1)
        for j = 1:size(labelled_ground_truth,2)
            % test sample vector
            vecX = double([r_img(i,j); g_img(i,j); b_img(i,j); fe_img(i,j); le_img(i,j); nir_img(i,j)]);
            % no of samples
            n = numel(vecX);
            % pdf values
            pdfXScore = ([]);
        
            for k = 1:4
                pdfX = gauss_distribution(n,vecX,muArray(:,k),cell2mat(covCell(k))); % class 1
                 % collect pdf Score for each class
                pdfXScore = [pdfXScore,pdfX]; 
            end
            % compare with ground truth
            index = find(pdfXScore == max(pdfXScore)); % identify the class
            predicted(i,j) = index;
        end
    end

%% confusion matrix
[C,order] = confusionmat(labelled_ground_truth(:), predicted(:),'Order',[4 3 2 1]);
cm = confusionchart(C);
classificationAccuracy = sum(diag(C))/sum(C,'all');

%% Plot Images

mymap = [1 1 0; % Yellow
         0 1 0; % green
         1 0 0; % red
         0 0 0 % black
         ];

figure;
subplot(1,2,1);
imshow (labelled_ground_truth,mymap);
title('Ground Truth')

subplot(1,2,2);
imshow (predicted,mymap);
title('Predicted')
