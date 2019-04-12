/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
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
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* To the the UUID (found the the TA's h-file(s)) */
#include <test_ta.h>

	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TEST_TA_UUID;
	uint32_t err_origin;
	int c = 1; 
	int n;

	TEEC_SharedMemory test_data;
	test_data.buffer = NULL;
	test_data.size = 64;
	test_data.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT;

void start_ta_session()
{

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

}

void finish_ta_session()
{
	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */

	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);
	
}

void invoke_ta_function()
{
	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */

	/* Clear the TEEC_Operation struct */
	memset(&op, 0, sizeof(op));

	/*
	 * Prepare the argument. Pass a value in the first parameter,
	 * the remaining three parameters are unused.
	 */
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = 42;

	/*
	 * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
	 * called.
	 */
	printf("Invoking TA to increment %d\n", op.params[0].value.a);
	res = TEEC_InvokeCommand(&sess, TA_HELLO_WORLD_CMD_INC_VALUE, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
	printf("TA incremented value to %d\n", op.params[0].value.a);
}

void random_number_generator()
{

	printf("Ten random numbers in [1,100]\n");
	if (c = 1) {
	n = rand() % 100 + 1;
	printf("%d\n", n);
	}
}

void sharedMemory()
{
/*
	TEEC_SharedMemory fim_hash;
	fim_hash.buffer = NULL;
	fim_hash.size = MD5_LEN + 1;
	fim_hash.flags = TEEC_MEM_INPUT;
	rc = TEEC_AllocateSharedMemory (&ctx , & fim_hash);
	memcpy (fim_hash.buffer , fim_md5 , fim_hash.size );
*/

	/*diese Variante ist nur eine andere Schreibweise als die da oben
	TEEC_SharedMemory buf_shm = {
	.flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT,
	.buffer = NULL,
	.size = 64 /* put size of the buffer you need */
/*	};*/

	res = TEEC_AllocateSharedMemory (&ctx, &test_data);
	if (res != TEEC_SUCCESS)
      	return;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
                                         TEEC_NONE, TEEC_NONE);

	op.params[1].memref.parent = &test_data;
}

void neustart()
{
	system("reboot");

}

int main(void)
{
	/*printf("wird es klappen\n");
	neustart();*/	
	
	printf("Session aufbauen\n");
	start_ta_session();

	printf("Funktion in ta ausführen\n");
	invoke_ta_function();

	printf("Zufällige Nummer generieren\n");
	random_number_generator();

	printf("sharedmemory\n");
	sharedMemory();

	printf("Session beenden\n");
	finish_ta_session();
	return 0;
}
