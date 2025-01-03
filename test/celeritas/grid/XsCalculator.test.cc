//----------------------------------*-C++-*----------------------------------//
// Copyright 2020-2024 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file celeritas/grid/XsCalculator.test.cc
//---------------------------------------------------------------------------//
#include "celeritas/grid/XsCalculator.hh"

#include <algorithm>
#include <cmath>

#include "corecel/cont/Range.hh"
#include "corecel/data/CollectionBuilder.hh"
#include "corecel/io/Repr.hh"

#include "CalculatorTestBase.hh"
#include "celeritas_test.hh"

namespace celeritas
{
namespace test
{
//---------------------------------------------------------------------------//
// TEST HARNESS
//---------------------------------------------------------------------------//

class XsCalculatorTest : public CalculatorTestBase
{
  protected:
    using Energy = XsCalculator::Energy;
};

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST_F(XsCalculatorTest, simple)
{
    // Energy from 1 to 1e5 MeV with 6 grid points; XS = E
    // *No* magical 1/E scaling
    this->build(1.0, 1e5, 6);

    XsCalculator calc(this->data(), this->values());

    // Test on grid points
    EXPECT_SOFT_EQ(1.0, calc(Energy{1}));
    EXPECT_SOFT_EQ(1e2, calc(Energy{1e2}));
    EXPECT_SOFT_EQ(1e5 - 1e-6, calc(Energy{1e5 - 1e-6}));
    EXPECT_SOFT_EQ(1e5, calc(Energy{1e5}));

    // Test access by index
    EXPECT_SOFT_EQ(1.0, calc[0]);
    EXPECT_SOFT_EQ(1e2, calc[2]);
    EXPECT_SOFT_EQ(1e5, calc[5]);

    // Test between grid points
    EXPECT_SOFT_EQ(5, calc(Energy{5}));

    // Test out-of-bounds
    EXPECT_SOFT_EQ(1.0, calc(Energy{0.0001}));
    EXPECT_SOFT_EQ(1e5, calc(Energy{1e7}));

    // Test energy grid bounds
    EXPECT_SOFT_EQ(1.0, value_as<Energy>(calc.energy_min()));
    EXPECT_SOFT_EQ(1e5, value_as<Energy>(calc.energy_max()));
}

TEST_F(XsCalculatorTest, scaled_lowest)
{
    // Energy from .1 to 1e4 MeV with 6 grid points and values of 1
    this->build({0.1, 1e4}, 6, [](real_type) { return real_type{1}; });
    this->convert_to_prime(0);

    XsCalculator calc(this->data(), this->values());

    // Test on grid points
    EXPECT_SOFT_EQ(1, calc(Energy{0.1}));
    EXPECT_SOFT_EQ(1, calc(Energy{1e2}));
    EXPECT_SOFT_EQ(1, calc(Energy{1e4 - 1e-6}));
    EXPECT_SOFT_EQ(1, calc(Energy{1e4}));

    // Test access by index
    EXPECT_SOFT_EQ(1, calc[0]);
    EXPECT_SOFT_EQ(1, calc[2]);
    EXPECT_SOFT_EQ(1, calc[5]);

    // Test between grid points
    EXPECT_SOFT_EQ(1, calc(Energy{0.2}));
    EXPECT_SOFT_EQ(1, calc(Energy{5}));

    // Test out-of-bounds: cross section still scales according to 1/E (TODO:
    // this might not be the best behavior for the lower energy value)
    EXPECT_SOFT_EQ(1000, calc(Energy{0.0001}));
    EXPECT_SOFT_EQ(0.1, calc(Energy{1e5}));

    // Test energy grid bounds
    EXPECT_SOFT_EQ(0.1, value_as<Energy>(calc.energy_min()));
    EXPECT_SOFT_EQ(1e4, value_as<Energy>(calc.energy_max()));
}

TEST_F(XsCalculatorTest, scaled_middle)
{
    // Energy from .1 to 1e4 MeV with 6 grid points
    this->build({0.1, 1e4}, 6, [](real_type) { return real_type{3}; });
    this->convert_to_prime(3);

    XsCalculator calc(this->data(), this->values());

    // Test on grid points
    EXPECT_SOFT_EQ(3, calc(Energy{0.1}));
    EXPECT_SOFT_EQ(3, calc(Energy{1e2}));
    EXPECT_SOFT_EQ(3, calc(Energy{1e4 - 1e-6}));
    EXPECT_SOFT_EQ(3, calc(Energy{1e4}));

    // Test access by index
    EXPECT_SOFT_EQ(3, calc[0]);
    EXPECT_SOFT_EQ(3, calc[2]);
    EXPECT_SOFT_EQ(3, calc[5]);

    // Test between grid points
    EXPECT_SOFT_EQ(3, calc(Energy{0.2}));
    EXPECT_SOFT_EQ(3, calc(Energy{5}));

    // Test out-of-bounds: cross section still scales according to 1/E
    EXPECT_SOFT_EQ(3, calc(Energy{0.0001}));
    EXPECT_SOFT_EQ(0.3, calc(Energy{1e5}));

    // Test energy grid bounds
    EXPECT_SOFT_EQ(0.1, value_as<Energy>(calc.energy_min()));
    EXPECT_SOFT_EQ(1e4, value_as<Energy>(calc.energy_max()));
}

TEST_F(XsCalculatorTest, scaled_linear)
{
    auto reference_xs = [](real_type energy) {
        auto result = 100 + energy * 10;
        if (energy > 1)
        {
            result *= 1 / energy;
        }
        return result;
    };

    this->build({1e-3, 1e3}, 7, reference_xs);
    this->convert_to_prime(3);

    XsCalculator interp_xs(this->data(), this->values());

    for (real_type e : {1e-3, 1e-1, 0.5, 1.0, 1.5, 10.0, 12.5, 1e3})
    {
        EXPECT_SOFT_EQ(reference_xs(e), interp_xs(Energy{e}))
            << "e=" << repr(e);
    }
}

TEST_F(XsCalculatorTest, scaled_highest)
{
    // values of 1, 10, 100 --> actual xs = {1, 10, 1}
    this->build(1, 100, 3);
    this->set_prime_index(2);

    XsCalculator calc(this->data(), this->values());
    EXPECT_SOFT_EQ(1, calc(Energy{0.0001}));
    EXPECT_SOFT_EQ(1, calc(Energy{1}));
    EXPECT_SOFT_EQ(10, calc(Energy{10}));
    EXPECT_SOFT_EQ(2.0, calc(Energy{90}));

    // Test access by index
    EXPECT_SOFT_EQ(1, calc[0]);
    EXPECT_SOFT_EQ(10, calc[1]);
    EXPECT_SOFT_EQ(1, calc[2]);

    // Final point and higher are scaled by 1/E
    EXPECT_SOFT_EQ(1, calc(Energy{100}));
    EXPECT_SOFT_EQ(.1, calc(Energy{1000}));

    // Test energy grid bounds
    EXPECT_SOFT_EQ(1, value_as<Energy>(calc.energy_min()));
    EXPECT_SOFT_EQ(100, value_as<Energy>(calc.energy_max()));
}

TEST_F(XsCalculatorTest, TEST_IF_CELERITAS_DEBUG(scaled_off_the_end))
{
    // values of 1, 10, 100 --> actual xs = {1, 10, 100}
    this->build(1, 100, 3);
    XsGridData data(this->data());
    data.prime_index = 3;  // disallowed

    EXPECT_THROW(XsCalculator(data, this->values()), DebugError);
}

//---------------------------------------------------------------------------//
}  // namespace test
}  // namespace celeritas
