
; ramintvecs.asm
;
;  Created on: Jun. 11, 2021
;      Author: Robert Taylor

;-------------------------------------------------------------------------------

; Table in RAM
	.sect ".ramIntvecs"
	.retain ".ramIntvecs"
	.arm

    .ref vPortSWI

	.def ramSWI
ramSWI
	ldr pc, ram_tab_swi

ram_tab_swi:
	.word vPortSWI
