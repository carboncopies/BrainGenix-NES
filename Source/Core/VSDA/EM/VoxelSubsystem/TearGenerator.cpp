//=================================//
// This file is part of BrainGenix //
//=================================//


// Standard Libraries (BG convention: use <> instead of "")
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cmath>

// Third-Party Libraries (BG convention: use <> instead of "")

// Internal Libraries (BG convention: use <> instead of "")
#include <VSDA/EM/VoxelSubsystem/ArrayGeneratorPool/Task.h>
#include <VSDA/EM/VoxelSubsystem/ShapeToVoxel/ShapeToVoxel.h>
#include <VSDA/EM/VoxelSubsystem/VoxelArrayGenerator.h>






namespace BG {
namespace NES {
namespace Simulator {

struct Point2D {
    int X;
    int Y;
};

float PointDistance(Point2D _A, Point2D _B) {
    return std::sqrt(std::pow((_B.X - _A.X), 2) + std::pow((_B.Y - _A.Y), 2));
}

Point2D GeneratePoint(std::mt19937& _Generator, std::uniform_int_distribution<>& _XDist, std::uniform_int_distribution<>& _YDist) {

    Point2D P;
    P.X = _XDist(_Generator);
    P.Y = _YDist(_Generator);

    return P;
}

std::vector<VoxelArrayGenerator::Task> GenerateTear(BG::Common::Logger::LoggingSystem* _Logger, MicroscopeParameters* _Params, VoxelArray* _Array, VSDA::WorldInfo _Info, int _ZHeight, int _Seed) {

    int NumSegments = 5;
    int MaxSegmentLength = 1000;
    int MinSegmentLength = 100;
    int PointJitterXMax = 100;
    int PointJitterXMin = -100;
    int PointJitterYMax = 100;
    int PointJitterYMin = -100;



    // Set Random Number Generator For This Tear
    std::mt19937 PointGenerator(_Seed);
    std::uniform_int_distribution<> XVoxelDistribution(0, _Array->GetX());
    std::uniform_int_distribution<> YVoxelDistribution(0, _Array->GetY());

    // Generate Start/End Point
    Point2D StartPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);
    Point2D EndPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);

    while ((PointDistance(StartPoint, EndPoint) > MaxSegmentLength) || (PointDistance(StartPoint, EndPoint) < MinSegmentLength)) {
        EndPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);
    }


    // Now that we have the start/end points, we will add intermediate points
    std::vector<Point2D> Points;

    int DeltaX = (EndPoint.X - StartPoint.X) / NumSegments;
    int DeltaY = (EndPoint.Y - StartPoint.Y) / NumSegments;
    for (unsigned int i = 0; i < NumSegments; i++) {
        Point2D ThisPoint = StartPoint;
        ThisPoint.X += i * DeltaX;
        ThisPoint.Y += i * DeltaY;
        Points.push_back(ThisPoint);
    }

    
    // Now, move each point randomly
    std::uniform_int_distribution<> XJitter(PointJitterXMin, PointJitterXMax);
    std::uniform_int_distribution<> YJitter(PointJitterYMin, PointJitterYMax);
    for (unsigned int i = 0; i < Points.size(); i++) {
        Points[i].X += XJitter(PointGenerator);
        Points[i].Y += YJitter(PointGenerator);
    }


    // Now, create the tasks
    std::vector<VoxelArrayGenerator::Task> Tasks;
    for (unsigned int i = 1; i < Points.size(); i++) {

        VoxelArrayGenerator::Task ThisTask;
        ThisTask.Array_ = _Array;
        ThisTask.Parameters_ = _Params;
        ThisTask.WorldInfo_ = _Info;
        ThisTask.IsLineTask = true;
        ThisTask.LineTaskZIndex = _ZHeight;
        ThisTask.LineTaskP1XIndex = Points[i - 1].X;
        ThisTask.LineTaskP1YIndex = Points[i - 1].Y;
        ThisTask.LineTaskP2XIndex = Points[i].X;
        ThisTask.LineTaskP2YIndex = Points[i].Y;
        ThisTask.LineTaskP1Thickness = 50;
        ThisTask.LineTaskP2Thickness = 20;

        Tasks.push_back(ThisTask);
    }

    return Tasks;

}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
