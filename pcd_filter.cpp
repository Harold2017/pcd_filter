//
// Created by Harold on 2021/8/20.
//

#include <open3d/Open3D.h>
#include <fstream>
#include <iostream>
#include <map>
#include "matheval.hpp"
#include "include/io/arg_parser.h"
#include "include/io/filesystem.h"
#include "include/stopwatch.h"
#include <omp.h>

// -i: input pcd filepath (.xyz/.xyzn)
// -o: output pcd filepath (.xyz/.xyzn)
// -f: filter condition ("z > 0.1 && x < 0.5")
int main(int argc, char* argv[]) {
    // pcd file io
    auto in_file = M_ARG_PARSER::ParseAsString(argc, argv, "-i", "");
    if (in_file.empty()) {
        std::cerr << "invalid input filename: " << in_file << std::endl;
        exit(1);
    }
    auto out_file = M_ARG_PARSER::ParseAsString(argc, argv, "-o", "");
    if (out_file.empty()) {
        std::cerr << "invalid output filename: " << in_file << std::endl;
        exit(1);
    }
    auto in_ext = M_FILESYSTEM::GetFileExtensionLower(in_file);
    std::shared_ptr<open3d::geometry::PointCloud> p_source = nullptr;

    {
        TIME_BLOCK("-read file");
        p_source = open3d::io::CreatePointCloudFromFile(in_file, in_ext);
        if (!p_source) {
            std::cerr << "can not read in file: " << in_file << std::endl;
            exit(1);
        }
    }

    // read in filter condition
    auto f_con = M_ARG_PARSER::ParseAsString(argc, argv, "-f", "");

    // filter
    open3d::geometry::PointCloud pcd;
    pcd.points_.reserve(p_source->points_.size());
    {
        TIME_BLOCK("-filter");
        auto const& pts = p_source->points_;
        auto & o_pts = pcd.points_;
        matheval::Parser parser;
        parser.parse(f_con);

/*
// to keep points push back in order
#pragma omp parallel
        {
            std::vector<Eigen::Vector3d> o_pts_private;
#pragma omp for nowait schedule(static)
            for (auto i = 0; i < pts.size(); i++) {
                std::map<std::string, double> symtab = {
                    { "x", pts[i].x() },
                    { "y", pts[i].y() },
                    { "z", pts[i].z() }
                };
                if (parser.evaluate(symtab))
                    o_pts_private.emplace_back(pts[i]);
            }
#pragma omp for schedule(static) ordered
            for(auto i = 0; i < omp_get_num_threads(); i++) {
#pragma omp ordered
                o_pts.insert(o_pts.end(), o_pts_private.begin(), o_pts_private.end());
            }
        }
*/

// MSVC only support omp 2.0 where only signed int is supported in loop, so disable omp in MSVC if the size is out of int range
#ifndef _WIN32
#pragma omp parallel for
#endif
        for (size_t i = 0; i < pts.size(); i++) {
            std::map<std::string, double> symtab = {
                { "x", pts[i].x() },
                { "y", pts[i].y() },
                { "z", pts[i].z() }
            };
            if (parser.evaluate(symtab))
                o_pts.emplace_back(pts[i]);
        }
    }
    
    // write output pcd file
    {
        TIME_BLOCK("-write file");
        auto ret = open3d::io::WritePointCloud(out_file, pcd);
        if (!ret) {
            std::cerr << "fail to write output file: " << out_file << std::endl;
            exit(1);
        }
    }

    return 0;
}