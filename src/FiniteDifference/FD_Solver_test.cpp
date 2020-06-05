

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "FD_Solver.hpp"
#include "Solvers/petsc_solver.hpp"
#include "FD_expressions.hpp"

//! Specify the general characteristic of system to solve
struct equations2d1 {

    //! dimensionaly of the equation ( 3D problem ...)
    static const unsigned int dims=2;
    //! number of fields in the system
    static const unsigned int nvar=1;

    //! boundary at X and Y
    static const bool boundary[];

    //! type of space float, double, ...
    typedef double stype;

    //! type of base particles
    typedef grid_dist_id<dims, double, aggregate<double,double,double>> b_part;

    //! type of SparseMatrix for the linear solver
    typedef SparseMatrix<double, int, PETSC_BASE> SparseMatrix_type;

    //! type of Vector for the linear solver
    typedef Vector<double, PETSC_BASE> Vector_type;

    typedef petsc_solver<double> solver_type;
};

BOOST_AUTO_TEST_SUITE( FD_Solver_test )


BOOST_AUTO_TEST_CASE(solver_check_diagonal)
{
	const size_t sz[2] = {81,81};
    Box<2, double> box({0, 0}, {1, 1});
    periodicity<2> bc = {NON_PERIODIC, NON_PERIODIC};
    Ghost<2,long int> ghost(1);

    grid_dist_id<2, double, aggregate<double,double,double>> domain(sz, box, ghost, bc);


    auto it = domain.getDomainIterator();
    while (it.isNext())
    {
    	auto key = it.get();
    	auto gkey = it.getGKey(key);
        double x = gkey.get(0) * domain.spacing(0);
        double y = gkey.get(1) * domain.spacing(1);
        domain.get<1>(key) = x+y;

        ++it;
    }

    domain.ghost_get<0>();

    auto v =  FD::getV<0>(domain);
    auto RHS= FD::getV<1>(domain);
    auto sol= FD::getV<2>(domain);

    FD_scheme<equations2d1,decltype(domain)> Solver(ghost,domain);

    Solver.impose(5.0*v,{0,0},{80,80}, prop_id<1>());
    Solver.solve(sol);


    domain.write("basic_test");
}

BOOST_AUTO_TEST_SUITE_END()
