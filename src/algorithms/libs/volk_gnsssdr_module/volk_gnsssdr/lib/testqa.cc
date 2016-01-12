/*
 * Copyright (C) 2010-2015 (see AUTHORS file for a list of contributors)
 *
 * This file is part of GNSS-SDR.
 *
 * GNSS-SDR is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNSS-SDR is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNSS-SDR. If not, see <http://www.gnu.org/licenses/>.
 */


#include "qa_utils.h"
#include "kernel_tests.h"

#include <volk_gnsssdr/volk_gnsssdr.h>

#include <vector>
#include <utility>
#include <iostream>
#include <fstream>

void print_qa_xml(std::vector<volk_gnsssdr_test_results_t> results, unsigned int nfails);

int main()
{
    bool qa_ret_val = 0;

    float def_tol = 1e-6;
    lv_32fc_t def_scalar = 327.0;
    int def_iter = 1;
    int def_vlen = 131071;
    bool def_benchmark_mode = true;
    std::string def_kernel_regex = "";

    volk_gnsssdr_test_params_t test_params(def_tol, def_scalar, def_vlen, def_iter,
        def_benchmark_mode, def_kernel_regex);
    std::vector<volk_gnsssdr_test_case_t> test_cases = init_test_list(test_params);

    std::vector<std::string> qa_failures;
    std::vector<volk_gnsssdr_test_results_t> results;
    // Test every kernel reporting failures when they occur
    for(unsigned int ii = 0; ii < test_cases.size(); ++ii) {
        bool qa_result = false;
        volk_gnsssdr_test_case_t test_case = test_cases[ii];
        try {
            qa_result = run_volk_gnsssdr_tests(test_case.desc(), test_case.kernel_ptr(), test_case.name(),
                test_case.test_parameters(), &results, test_case.puppet_master_name());
        }
        catch(...) {
            // TODO: what exceptions might we need to catch and how do we handle them?
            std::cerr << "Exception found on kernel: " << test_case.name() << std::endl;
            qa_result = false;
        }

        if(qa_result) {
            std::cerr << "Failure on " << test_case.name() << std::endl;
            qa_failures.push_back(test_case.name());
        }
    }

    // Generate XML results
    print_qa_xml(results, qa_failures.size());

    // Summarize QA results
    std::cerr << "Kernel QA finished: " << qa_failures.size() << " failures out of "
        << test_cases.size() << " tests." << std::endl;
    if(qa_failures.size() > 0) {
        std::cerr << "The following kernels failed QA:" << std::endl;
        for(unsigned int ii = 0; ii < qa_failures.size(); ++ii) {
            std::cerr << "    " << qa_failures[ii] << std::endl;
        }
        qa_ret_val = 1;
    }

    return qa_ret_val;
}

/*
 * This function prints qa results as XML output similar to output
 * from Junit. For reference output see http://llg.cubic.org/docs/junit/
 */
void print_qa_xml(std::vector<volk_gnsssdr_test_results_t> results, unsigned int nfails)
{
    std::ofstream qa_file;
    qa_file.open(".unittest/kernels.xml");

    qa_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    qa_file << "<testsuites name=\"kernels\" " <<
        "tests=\"" << results.size() << "\" " <<
        "failures=\"" << nfails << "\" id=\"1\">" << std::endl;

    // Results are in a vector by kernel. Each element has a result
    // map containing time and arch name with test result
    for(unsigned int ii=0; ii < results.size(); ++ii) {
        volk_gnsssdr_test_results_t result = results[ii];
        qa_file << "  <testsuite name=\"" << result.name << "\">" << std::endl;

        std::map<std::string, volk_gnsssdr_test_time_t>::iterator kernel_time_pair;
        for(kernel_time_pair = result.results.begin(); kernel_time_pair != result.results.end(); ++kernel_time_pair) {
            volk_gnsssdr_test_time_t test_time = kernel_time_pair->second;
            qa_file << "    <testcase name=\"" << test_time.name << "\" " <<
                "classname=\"" << result.name << "\" " <<
                "time=\"" << test_time.time << "\">" << std::endl;
            if(!test_time.pass)
                qa_file << "      <failure " <<
                    "message=\"fail on arch " <<  test_time.name << "\">" <<
                    "</failure>" << std::endl;
            qa_file << "    </testcase>" << std::endl;
        }
        qa_file << "  </testsuite>" << std::endl;
    }


    qa_file << "</testsuites>" << std::endl;
    qa_file.close();

}







//VOLK_RUN_TESTS(volk_gnsssdr_16i_x5_add_quad_16i_x4, 1e-4, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_branch_4_state_8, 1e-4, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_max_star_16i, 0, 0, 20462, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_max_star_horizontal_16i, 0, 0, 20462, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_permute_and_scalar_add, 1e-4, 0, 2046, 1000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_x4_quad_max_star_16i, 1e-4, 0, 2046, 1000);
//VOLK_RUN_TESTS(volk_gnsssdr_16i_32fc_dot_prod_32fc, 1e-4, 0, 204602, 1);
//VOLK_RUN_TESTS(volk_gnsssdr_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_32fc_s32f_x2_power_spectral_density_32f, 1e-4, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_32f_s32f_32f_fm_detect_32f, 1e-4, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_32u_popcnt, 0, 0, 2046, 10000);
//VOLK_RUN_TESTS(volk_gnsssdr_64u_popcnt, 0, 0, 2046, 10000);
