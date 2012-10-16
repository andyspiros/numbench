//=====================================================
// Copyright (C) 2012 Andrea Arteaga <andyspiros@gmail.com>
//=====================================================
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#ifndef ACCURACY_HPP
#define ACCURACY_HPP

#include "timers/LinuxTimer.hpp"

#include <string>
#include <vector>
#include <cmath>

template<class Action>
void bench_accuracy (int size_min, int size_max, int nb_point,
                     bool silent = false)
{
    std::string filename = "accuracy_"+Action::name()+".dat";

    if (!silent)
        std::cout << "starting " << filename << std::endl;

    // Initialization vectors
    std::vector<double> errors(nb_point);
    std::vector<double> devs(nb_point);
    std::vector<int> sizes(nb_point);

    // Initialization timer
    LinuxTimer timer;

    // Log-distributed sizes
    size_lin_log(nb_point, size_min, size_max, sizes);

    // Loop on sizes
    for (int i = nb_point-1; i >= 0; --i) {
        const int size = sizes[i];

        if (!silent)
          std::cout << " " << "size = " << size << ", " << std::flush;

        int repetitions = 0;
        double average = 0., stddev = 0., e;

        // Perform time loop and store average and standard deviation
        timer.start();
        do {
            // Initialize action with given size and new seed
            Action action(size,  15+repetitions);
            e = action.getResidual();

            average += e;
            stddev += e*e;
            ++repetitions;

        } while(timer.elapsed() < 1. || repetitions < 4);

        // Compute average and standard deviation
        // (sometimes strange things happen)
        average /= repetitions;
        stddev = std::sqrt(std::fabs(stddev/repetitions - average*average));

        errors[i] = average;
        devs[i] = stddev;

        // Output
        if (!silent)
            std::cout << "average = " << average << ",  stddev = " << stddev
                      << " (" << repetitions << " samples)" << std::endl;
    }

    // Dump the result
    if (!silent) {
        std::ofstream outfile(filename.c_str());

        // Don't dump the stddev for now
        for (int i = 0; i < nb_point; ++i)
            //outfile << sizes[i] << " " << errors[i] << " " << devs[i] << "\n";
            outfile << sizes[i] << " " << errors[i] << "\n";

        outfile.close();

    }
}

#endif // ACCURACY_HPP
