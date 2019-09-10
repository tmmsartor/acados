/*
 * Copyright 2019 Gianluca Frison, Dimitris Kouzoupis, Robin Verschueren,
 * Andrea Zanelli, Niels van Duijkeren, Jonathan Frey, Tommaso Sartor,
 * Branimir Novoselnik, Rien Quirynen, Rezart Qelibari, Dang Doan,
 * Jonas Koenemann, Yutao Chen, Tobias Schöls, Jonas Schlagenhauf, Moritz Diehl
 *
 * This file is part of acados.
 *
 * The 2-Clause BSD License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.;
 */

#include "mex.h"


#define MEX_DIM_CHECK(fun_name, field, matlab_size, acados_size) {\
	if (acados_size != matlab_size)\
	{\
		sprintf(buffer, "%s: error setting %s, wrong dimension, got %d, need %d", fun_name, field, matlab_size, acados_size);\
		mexErrMsgTxt(buffer);\
	}\
}

#define MEX_SETTER_NO_STAGE_SUPPORT(fun_name, field) {\
	sprintf(buffer, "%s setting %s for specific stage not supported", fun_name, field);\
	mexErrMsgTxt(buffer);\
}

#define MEX_FIELD_NOT_SUPPORTED(fun_name, field) {\
	sprintf(buffer, "%s field %s not supported", fun_name, field);\
	mexErrMsgTxt(buffer);\
}

#define MEX_FIELD_NOT_SUPPORTED_FOR_COST_STAGE(fun_name, field, cost, ii) {\
	sprintf(buffer, "%s field %s not supported for cost type %d at stage %d", fun_name, field, cost, ii);\
	mexErrMsgTxt(buffer);\
}



// macro to string
#define STR(x) STR_AGAIN(x)
#define STR_AGAIN(x) #x

// glue macros
#define GLUE2(x,y) GLUE2_AGAIN(x,y)
#define GLUE2_AGAIN(x,y) x##y

// macro bricks
#define WORK _work
#define SP_IN _sparsity_in
#define SP_OUT _sparsity_out
#define N_IN _n_in
#define N_OUT _n_out
