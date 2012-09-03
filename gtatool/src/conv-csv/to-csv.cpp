/*
 * This file is part of gtatool, a tool to manipulate Generic Tagged Arrays
 * (GTAs).
 *
 * Copyright (C) 2012
 * Martin Lambers <marlam@marlam.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <string>
#include <limits>
#include <cstdio>
#include <cstring>

#include <gta/gta.hpp>

#include "msg.h"
#include "str.h"
#include "fio.h"
#include "opt.h"

#include "lib.h"

#include "delimiter.h"


extern "C" void gtatool_to_csv_help(void)
{
    msg::req_txt("to-csv [-D|--delimiter=D] [<input-file>] <output-file>\n"
            "\n"
            "Converts GTAs to csv format, using the field delimiter D. "
            "D is a single ASCII character; the default is the comma (',').");
}

extern "C" int gtatool_to_csv(int argc, char *argv[])
{
    std::vector<opt::option *> options;
    opt::info help("help", '\0', opt::optional);
    options.push_back(&help);
    std::vector<std::string> delimiters = gta_csv_create_delimiters();
    opt::string delimiter("delimiter", 'D', opt::optional, delimiters, std::string(","));
    options.push_back(&delimiter);
    std::vector<std::string> arguments;
    if (!opt::parse(argc, argv, options, 1, 2, arguments))
    {
        return 1;
    }
    if (help.value())
    {
        gtatool_to_csv_help();
        return 0;
    }

    try
    {
        std::string nameo = arguments.size() == 1 ? arguments[0] : arguments[1];
        array_loop_t array_loop;
        gta::header hdr;
        std::string name;

        array_loop.start(arguments.size() == 1 ? std::vector<std::string>() : std::vector<std::string>(1, arguments[0]), nameo);
        while (array_loop.read(hdr, name))
        {
            if (hdr.dimensions() != 1 && hdr.dimensions() != 2)
            {
                throw exc(name + ": only one- or two-dimensional arrays can be converted to CSV.");
            }
            if (hdr.components() < 1)
            {
                throw exc(name + ": unsupported number of element components.");
            }
            for (uintmax_t c = 0; c < hdr.components(); c++)
            {
                if (hdr.component_type(c) != gta::int8
                        && hdr.component_type(c) != gta::uint8
                        && hdr.component_type(c) != gta::int16
                        && hdr.component_type(c) != gta::uint16
                        && hdr.component_type(c) != gta::int32
                        && hdr.component_type(c) != gta::uint32
                        && hdr.component_type(c) != gta::int64
                        && hdr.component_type(c) != gta::uint64
                        && hdr.component_type(c) != gta::float32
                        && hdr.component_type(c) != gta::float64)
                {
                    throw exc(name + ": unsupported element component type(s).");
                }
            }

            FILE *fo = fio::open(nameo, "w");
            element_loop_t element_loop;
            array_loop.start_element_loop(element_loop, hdr, gta::header());
            for (uintmax_t e = 0; e < hdr.elements(); e++)
            {
                const void *p = element_loop.read();
                for (uintmax_t c = 0; c < hdr.components(); c++)
                {
                    std::string s;
                    if (hdr.component_type(c) == gta::int8)
                    {
                        int8_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(int8_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::uint8)
                    {
                        uint8_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(uint8_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::int16)
                    {
                        int16_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(int16_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::uint16)
                    {
                        uint16_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(uint16_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::int32)
                    {
                        int32_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(int32_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::uint32)
                    {
                        uint32_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(uint32_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::int64)
                    {
                        int64_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(int64_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::uint64)
                    {
                        uint64_t v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(uint64_t));
                        s = str::from(v);
                    }
                    else if (hdr.component_type(c) == gta::float32)
                    {
                        float v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(float));
                        s = str::from(v);
                    }
                    else // hdr.component_type(c) == gta::float64
                    {
                        double v;
                        std::memcpy(&v, hdr.component(p, c), sizeof(double));
                        s = str::from(v);
                    }
                    if (std::fputs(s.c_str(), fo) == EOF)
                    {
                        throw exc(nameo + ": output error.");
                    }
                    if (c < hdr.components() - 1 && std::fputs(delimiter.value().c_str(), fo) == EOF)
                    {
                        throw exc(nameo + ": output error.");
                    }
                }
                if (e == hdr.elements() - 1 ||
                        (hdr.dimensions() == 2 && e % hdr.dimension_size(0) == hdr.dimension_size(0) - 1))
                {
                    if (std::fputs("\r\n", fo) == EOF)
                    {
                        throw exc(nameo + ": output error.");
                    }
                }
                else
                {
                    if (std::fputs(delimiter.value().c_str(), fo) == EOF)
                    {
                        throw exc(nameo + ": output error.");
                    }
                }
            }
            fio::flush(fo, nameo);
            if (std::ferror(fo))
            {
                throw exc(nameo + ": output error.");
            }
            fio::close(fo, nameo);
        }
        array_loop.finish();
    }
    catch (std::exception &e)
    {
        msg::err_txt("%s", e.what());
        return 1;
    }

    return 0;
}