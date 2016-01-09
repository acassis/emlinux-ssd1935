#ifndef _EBM_MEM_SASI_
#define _EBM_MEM_SASI_


void	*ebm_malloc(int size);
/**<
allocate memory from common pool on FPGA
@param[in]	size	bytes
@return		address allocated on success, 0 on failure
*/

void	*ebm_malloca(int size, int align);
/**<
allocate memory from common pool on FPGA
@param[in]	size	bytes
@param[in]	align	start address alignment in bytes
@return		address allocated on success, 0 on failure
*/

int		ebm_mfree(void *p);
/**<
free memory to common pool on FPGA
@param[in]	p		address allocated by ebm_malloc
@return		0 on success
*/


#define ebm_malloc(size)	ebm_malloca(size, 1)


#endif

