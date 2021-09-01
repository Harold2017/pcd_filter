//
// Created by Harold on 2021/8/20.
//

#include <open3d/Open3D.h>
#include <fstream>
#include <iostream>
#include <map>
#include "matheval.hpp"
#include "include/io/arg_parser.h"
#include "include/stopwatch.h"
#include <omp.h>

// -i: input stl filepath (.ply/.stl/.obj)
// -o: output stl filepath (.ply/.stl/.obj)
// -f: filter condition ("z > 0.1 && x < 0.5")
int main(int argc, char* argv[]) {
    // mesh file io
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
    std::shared_ptr<open3d::geometry::TriangleMesh> p_source = nullptr;
    {
        TIME_BLOCK("-read file");
        p_source = open3d::io::CreateMeshFromFile(in_file);
        if (!p_source) {
            std::cerr << "can not read in file: " << in_file << std::endl;
            exit(1);
        }
    }

    // read in filter condition
    auto f_con = M_ARG_PARSER::ParseAsString(argc, argv, "-f", "");

    // filter
    std::shared_ptr<open3d::geometry::TriangleMesh> mesh = nullptr;
    {
        TIME_BLOCK("-filter");
        auto const& pts = p_source->vertices_;
        std::vector<size_t> indices;
		indices.reserve(pts.size());
        matheval::Parser parser;
        parser.parse(f_con);

#pragma omp parallel for
        for (auto i = 0; i < pts.size(); i++) {
            std::map<std::string, double> symtab = {
                { "x", pts[i].x() },
                { "y", pts[i].y() },
                { "z", pts[i].z() }
            };
            if (parser.evaluate(symtab))
                indices.emplace_back(i);
        }
        mesh = p_source->SelectByIndex(indices);
    }
    
    // write output mesh file
    {
        TIME_BLOCK("-write file");
        auto ret = open3d::io::WriteTriangleMesh(out_file, *mesh);
        if (!ret) {
            std::cerr << "fail to write output file: " << out_file << std::endl;
            exit(1);
        }
    }

    return 0;
}