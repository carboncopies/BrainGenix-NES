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

enum TearOrigin {
    TEAR_TOP,
    TEAR_BOTTOM
};

int GenerateTear(BG::Common::Logger::LoggingSystem* _Logger, std::vector<std::unique_ptr<VoxelArrayGenerator::Task>>& _TaskList, VoxelArrayGenerator::ArrayGeneratorPool* _GeneratorPool, ScanRegion _Region, MicroscopeParameters* _Params, VoxelArray* _Array, VSDA::WorldInfo _Info, int _ZHeight, int _Seed) {

    int NumSegments = _Params->TearNumSegments;
    int MinSegmentLength = _Params->TearMinimumLength_um / _Params->VoxelResolution_um;
    int PointJitterXMax = _Params->TearPointJitterXMax_um / _Params->VoxelResolution_um;
    int PointJitterXMin = _Params->TearPointJitterXMin_um / _Params->VoxelResolution_um;
    int PointJitterYMax = _Params->TearPointJitterYMax_um / _Params->VoxelResolution_um;
    int PointJitterYMin = _Params->TearPointJitterYMin_um / _Params->VoxelResolution_um;
    float StartSize = _Params->TearStartSize_um;
    float EndSize = _Params->TearEndSize_um;
    int MaxDeltaY = _Params->TearMaxDeltaY_um / _Params->VoxelResolution_um;
    int MaxDeltaX = _Params->TearMaxDeltaX_um / _Params->VoxelResolution_um;


    // Set Random Number Generator For This Tear
    std::mt19937 PointGenerator(_Seed);
    std::uniform_int_distribution<> XVoxelDistribution(0, _Array->GetX());
    std::uniform_int_distribution<> YVoxelDistribution(0, _Array->GetY());

    // Pick if this tear starts at the top or bottom of the slice
    TearOrigin Origin = TearOrigin(PointGenerator() % 2);


    // Generate Start/End Point
    Point2D StartPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);
    Point2D EndPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);

    // Set the originating point to top or bottom of the sample
    if (Origin == TEAR_TOP) {
        StartPoint.Y = -100;
    } else {
        StartPoint.Y = _Array->GetY() + 100;
    }


    // Prevent hangs if this is unable to generate properly due to a small output or something
    int MaxIters = 1000000;
    int NumIters = 0;
    while ((NumIters < MaxIters) && ((abs(EndPoint.X - StartPoint.X) > MaxDeltaX) || (PointDistance(StartPoint, EndPoint) < MinSegmentLength) || (abs(EndPoint.Y - StartPoint.Y) > MaxDeltaY))) {
        EndPoint = GeneratePoint(PointGenerator, XVoxelDistribution, YVoxelDistribution);
        NumIters++;
    }
    if (NumIters == MaxIters) {
        _Logger->Log("Unable to generate sample tear correctly, hit iteration limit when selecting point for tear to go to, try lowering your MinSegmentLength", 8);
    }


    // Now that we have the start/end points, we will add intermediate points
    std::vector<Point2D> Points;
    std::vector<float> Sizes;
    int DeltaX = (EndPoint.X - StartPoint.X) / NumSegments;
    int DeltaY = (EndPoint.Y - StartPoint.Y) / NumSegments;
    for (unsigned int i = 0; i <= NumSegments; i++) {
        Point2D ThisPoint = StartPoint;
        ThisPoint.X += i * DeltaX;
        ThisPoint.Y += i * DeltaY;
        Points.push_back(ThisPoint);
    }

    
    // Now, move each point randomly
    std::uniform_int_distribution<> XJitter(PointJitterXMin, PointJitterXMax);
    std::uniform_int_distribution<> YJitter(PointJitterYMin, PointJitterYMax);
    float WidthStepSize = (EndSize - StartSize) / Points.size();
    for (unsigned int i = 0; i < Points.size(); i++) {
        Points[i].X += XJitter(PointGenerator);
        Points[i].Y += YJitter(PointGenerator);
        Sizes.push_back(StartSize + i*WidthStepSize);
    }


    // Now, create the tasks
    int NumShapes = 0;
    for (unsigned int i = 1; i < Points.size(); i++) {

        _TaskList.push_back(std::make_unique<VoxelArrayGenerator::Task>());
        NumShapes++;

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


        Geometries::Wedge& ThisWedge = ThisTask->ThisWedge;
        ThisWedge.End0Pos_um = End0;
        ThisWedge.End1Pos_um = End1;
        ThisWedge.End0Height_um = Sizes[i - 1];//_Info.VoxelScale_um;
        ThisWedge.End1Height_um = Sizes[i];//_Info.VoxelScale_um;
        ThisWedge.End0Width_um = _Info.VoxelScale_um*1.1;
        ThisWedge.End1Width_um = _Info.VoxelScale_um*1.1;


        _GeneratorPool->QueueWorkOperation(ThisTask);

    }


    return NumShapes;

}




}; // Close Namespace Logger
}; // Close Namespace Common
}; // Close Namespace BG
