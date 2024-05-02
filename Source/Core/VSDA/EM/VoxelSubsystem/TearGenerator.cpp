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

void GenerateTear(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<VoxelArrayGenerator::Task>>& _TaskList, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool, ScanRegion _Region, MicroscopeParameters* _Params, VoxelArray* _Array, VSDA::WorldInfo _Info, int _ZHeight, int _Seed) {

    int NumSegments = 2;
    int MaxSegmentLength = 1000;
    int MinSegmentLength = 100;
    int PointJitterXMax = 1;
    int PointJitterXMin = -1;
    int PointJitterYMax = 1;
    int PointJitterYMin = -1;



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
    std::cout<<"Layer: "<<_ZHeight<<std::endl;
    for (unsigned int i = 1; i < Points.size(); i++) {

        _TaskList.push_back(std::make_unique<VoxelArrayGenerator::Task>());


        VoxelArrayGenerator::Task* ThisTask = _TaskList[_TaskList.size() - 1].get();
        ThisTask->Array_ = _Array;
        ThisTask->Parameters_ = _Params;
        ThisTask->WorldInfo_ = _Info;
        ThisTask->CustomShape_ = VoxelArrayGenerator::CUSTOM_WEDGE;


        Geometries::Vec3D End0;
        End0.x = _Region.Point1X_um + (Points[i - 1].X * _Info.VoxelScale_um);
        End0.y = _Region.Point1Y_um + (Points[i - 1].Y * _Info.VoxelScale_um);
        End0.z = _Region.Point1Z_um + (_ZHeight * _Info.VoxelScale_um);

        Geometries::Vec3D End1;
        End1.x = _Region.Point1X_um + (Points[i].X * _Info.VoxelScale_um);
        End1.y = _Region.Point1Y_um + (Points[i].Y * _Info.VoxelScale_um);
        End1.z = _Region.Point1Z_um + (_ZHeight * _Info.VoxelScale_um);

        std::cout<<"    -P1: "<<End0.str()<<" |P2: "<<End1.str()<<std::endl;


        Geometries::Wedge& ThisWedge = ThisTask->ThisWedge;
        ThisWedge.End0Pos_um = End0;
        ThisWedge.End1Pos_um = End1;
        ThisWedge.End0Height_um = 0.1;//_Info.VoxelScale_um;
        ThisWedge.End1Height_um = 0.05;//_Info.VoxelScale_um;
        ThisWedge.End0Width_um = _Info.VoxelScale_um;
        ThisWedge.End1Width_um = _Info.VoxelScale_um;


        _GeneratorPool->QueueWorkOperation(ThisTask);

    }


}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
