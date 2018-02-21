/*
 *    This file is part of acados.
 *
 *    acados is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    acados is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with acados; if not, write to the Free Software Foundation,
 *    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "acados_c/dense_qp.h"

//external
#include <stdlib.h>
#include <assert.h>
#include <string.h>
//acados
#include <acados/dense_qp/dense_qp_common.h>
//acados_c
#include "acados_c/dense_qp/dense_qp_hpipm.h"
#ifdef ACADOS_WITH_QORE
#include "acados_c/dense_qp/dense_qp_qore.h"
#endif
#include "acados_c/dense_qp/dense_qp_qpoases.h"



void dense_qp_copy_dims(dense_qp_dims *dest, dense_qp_dims *src)
{
	dest->nv = src->nv;
	dest->ne = src->ne;
	dest->nb = src->nb;
	dest->ng = src->ng;
	dest->ns = src->ns;
	dest->memsize = src->memsize;
}



dense_qp_dims *create_dense_qp_dims()
{
    int bytes = dense_qp_dims_calculate_size();

    void *ptr = malloc(bytes);

    dense_qp_dims *dims = assign_dense_qp_dims(ptr);

    return dims;
}



dense_qp_in *create_dense_qp_in(dense_qp_dims *dims)
{
    int bytes = dense_qp_in_calculate_size(dims);

    void *ptr = malloc(bytes);

    dense_qp_in *in = assign_dense_qp_in(dims, ptr);

    return in;
}



dense_qp_out *create_dense_qp_out(dense_qp_dims *dims)
{
    int bytes = dense_qp_out_calculate_size(dims);

    void *ptr = malloc(bytes);

    dense_qp_out *out = assign_dense_qp_out(dims, ptr);

    return out;
}



int dense_qp_calculate_args_size(dense_qp_solver_plan *plan, dense_qp_dims *dims)
{
    dense_qp_solver_config fcn_ptrs = {
        .fun = NULL,
        .opts_calculate_size = NULL,
        .opts_assign = NULL,
        .opts_initialize_default = NULL,
        .memory_calculate_size = NULL,
        .memory_assign = NULL,
        .workspace_calculate_size = NULL};

    set_dense_qp_solver_fcn_ptrs(plan, &fcn_ptrs);

    int size = fcn_ptrs.opts_calculate_size(dims);

    return size;
}



void *dense_qp_assign_args(dense_qp_solver_plan *plan, dense_qp_dims *dims, void *raw_memory)
{
    dense_qp_solver_config fcn_ptrs = {
        .fun = NULL,
        .opts_calculate_size = NULL,
        .opts_assign = NULL,
        .opts_initialize_default = NULL,
        .memory_calculate_size = NULL,
        .memory_assign = NULL,
        .workspace_calculate_size = NULL};

    set_dense_qp_solver_fcn_ptrs(plan, &fcn_ptrs);

    void *args = fcn_ptrs.opts_assign(dims, raw_memory);

    fcn_ptrs.opts_initialize_default(args);

    return args;
}



void *dense_qp_create_args(dense_qp_solver_plan *plan, dense_qp_dims *dims)
{
    int bytes = dense_qp_calculate_args_size(plan, dims);

    void *ptr = malloc(bytes);

    void *args = dense_qp_assign_args(plan, dims, ptr);

    return args;
}



void *dense_qp_copy_args(dense_qp_solver_plan *plan, dense_qp_dims *dims, void *raw_memory, void *source)
{
    dense_qp_solver_t solver_name = plan->qp_solver;

    void *args;
    args = NULL;

    switch (solver_name)
    {
        case DENSE_QP_HPIPM:
            args = dense_qp_hpipm_copy_args(dims, raw_memory, source);
            break;
        case DENSE_QP_QORE:
            #ifdef ACADOS_WITH_QORE
            args = dense_qp_qore_copy_args(dims, raw_memory, source);
            #endif
            break;
        case DENSE_QP_QPOASES:
            args = dense_qp_qpoases_copy_args(dims, raw_memory, source);
            break;
        default:
            args = NULL;
            break;
    }

    return args;
}



int dense_qp_calculate_size(dense_qp_solver_plan *plan, dense_qp_dims *dims, void *args_)
{
    dense_qp_solver_config fcn_ptrs = {
        .fun = NULL,
        .opts_calculate_size = NULL,
        .opts_assign = NULL,
        .opts_initialize_default = NULL,
        .memory_calculate_size = NULL,
        .memory_assign = NULL,
        .workspace_calculate_size = NULL};

    set_dense_qp_solver_fcn_ptrs(plan, &fcn_ptrs);

    int bytes = 0;

    bytes += sizeof(dense_qp_solver);

    bytes += sizeof(dense_qp_solver_config);

    bytes += dense_qp_dims_calculate_size();

    bytes += fcn_ptrs.opts_calculate_size(dims);

    bytes += fcn_ptrs.memory_calculate_size(dims, args_);

    bytes += fcn_ptrs.workspace_calculate_size(dims, args_);

    return bytes;
}



dense_qp_solver *dense_qp_assign(dense_qp_solver_plan *plan, dense_qp_dims *dims, void *args_, void *raw_memory)
{
    char *c_ptr = (char *) raw_memory;

    dense_qp_solver *solver = (dense_qp_solver *) c_ptr;
    c_ptr += sizeof(dense_qp_solver);

    solver->fcn_ptrs = (dense_qp_solver_config *) c_ptr;
    c_ptr += sizeof(dense_qp_solver_config);
    set_dense_qp_solver_fcn_ptrs(plan, solver->fcn_ptrs);

    solver->dims = assign_dense_qp_dims(c_ptr);
    c_ptr += dense_qp_dims_calculate_size();
    dense_qp_copy_dims(solver->dims, dims);

    solver->args = dense_qp_copy_args(plan, dims, c_ptr, args_);
    c_ptr += solver->fcn_ptrs->opts_calculate_size(dims);

    solver->mem = solver->fcn_ptrs->memory_assign(dims, args_, c_ptr);
    c_ptr += solver->fcn_ptrs->memory_calculate_size(dims, args_);

    solver-> work = (void *) c_ptr;
    c_ptr += solver->fcn_ptrs->workspace_calculate_size(dims, args_);

    assert((char*)raw_memory + dense_qp_calculate_size(plan, dims, args_) == c_ptr);

    return solver;
}



dense_qp_solver *dense_qp_create(dense_qp_solver_plan *plan, dense_qp_dims *dims, void *args_)
{
    int bytes = dense_qp_calculate_size(plan, dims, args_);

    void *ptr = malloc(bytes);

    dense_qp_solver *solver = dense_qp_assign(plan, dims, args_, ptr);

    return solver;
}



int dense_qp_solve(dense_qp_solver *solver, dense_qp_in *qp_in, dense_qp_out *qp_out)
{
    return solver->fcn_ptrs->fun(qp_in, qp_out, solver->args, solver->mem, solver->work);
}



int set_dense_qp_solver_fcn_ptrs(dense_qp_solver_plan *plan, dense_qp_solver_config *fcn_ptrs)
{
    int return_value = ACADOS_SUCCESS;
    dense_qp_solver_t solver_name = plan->qp_solver;

    switch (solver_name)
    {
        case DENSE_QP_HPIPM:
			dense_qp_hpipm_config_initialize_default(fcn_ptrs);
            break;
        case DENSE_QP_QORE:
            #ifdef ACADOS_WITH_QORE
			dense_qp_qore_config_initialize_default(fcn_ptrs);
            #else
            return_value = ACADOS_FAILURE;
            #endif
            break;
        case DENSE_QP_QPOASES:
			dense_qp_qpoases_config_initialize_default(fcn_ptrs);
            break;
        default:
            return_value = ACADOS_FAILURE;
    }

    return return_value;
}
