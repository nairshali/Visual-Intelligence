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
 
 % % validation with training data 
   confusionTrainMatrix = ([]);
   n = 30;

   for cl = 1:4 %4 class 
    
        class1cnt = 0;
        class2cnt = 0;
        class3cnt = 0;
        class4cnt = 0;
        
        for feature = 1:size(cell2mat(SampleCell(:,:,cl)),2) % each feature 6
            pixel = cell2mat(SampleCell(:,feature,cl));
            
            for p = 1:size(cell2mat(SampleCell(:,:,1)),1) % each pixel within that feature, 40 pixel

                % establish guassian model for each class
                pdf1 = gauss_distribution(n,pixel(p),muArray(1,:),cell2mat(covCell(1))); % class 1
                pdf2 = gauss_distribution(n,pixel(p),muArray(2,:),cell2mat(covCell(2))); % class 2
                pdf3 = gauss_distribution(n,pixel(p),muArray(3,:),cell2mat(covCell(3))); % class 3
                pdf4 = gauss_distribution(n,pixel(p),muArray(4,:),cell2mat(covCell(4))); % class 4
                
                val = max([pdf1,pdf2,pdf3,pdf4]);

                if val == pdf1 
                   class1cnt =  class1cnt + 1;
                elseif val == pdf2 
                    class2cnt = class2cnt + 1;
                elseif val == pdf3 
                    class3cnt = class3cnt + 1;
                elseif val == pdf4
                    class4cnt = class4cnt + 1;
                end
            end    
        end  

        confusionTrainMatrix(cl,1) = class1cnt;
        confusionTrainMatrix(cl,2) = class2cnt;
        confusionTrainMatrix(cl,3) = class3cnt;
        confusionTrainMatrix(cl,4) = class4cnt;
   end
   
   classificationAccuracy = sum(diag(confusionMatrix))/sum(confusionMatrix,'all'); 
