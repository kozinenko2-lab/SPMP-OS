#include "mmi_flag_model.h"
#include <assert.h>
#include <stdio.h>
int main(void)
{
    uint32_t b=5;
    assert(mmi_flag_poll_model(&b,1,3)==5 && b==0); /* Returns and clears ALL flags. */
    b=5;assert(mmi_flag_poll_model(&b,3,0)==0 && b==5); /* AND not satisfied. */
    b=5;assert(mmi_flag_poll_model(&b,3,2)==5 && b==5); /* OR, no clear. */
    b=7;assert(mmi_flag_poll_model(&b,3,1)==7 && b==0); /* AND and clear. */
    for(unsigned m=0;m<4;m++) {
        b=7;assert(mmi_flag_poll_model(&b,0,m)==0 && b==7);
        b=0;assert(mmi_flag_poll_model(&b,1,m)==0 && b==0);
    }
    puts("PASS: OEM flag modes, return value and full clear semantics");
    return 0;
}
