#ifndef _INTCK_SASI_
#define _INTCK_SASI_


typedef irqreturn_t (*intc_isr_fn)(int cpl, void *dev_id, struct pt_regs *regs);

int	intc_req(int intr, intc_isr_fn isr, int type, const char *name, void *ctx);
int intc_free(int intr, void *ctx);
void intc_enable(int intr);
void intc_disable(int intr);


#endif

