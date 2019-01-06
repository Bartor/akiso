                opt f-g-h+l+o+
                org $1000

start           equ *

                lda <text
                sta $80
                lda >text
                sta $81
                ldy #1
                lda #$e5
                jsr phex

                lda <text
                ldx >text
                jsr $ff80
                brk

phex            pha
                jsr pxdig
                pla
                lsr @
                lsr @
                lsr @
                lsr @

pxdig           and #%00001111
                ora #'0'
                cmp #'9'+1
                bcc pr
                ADC #'A'-'9'-2

pr              sta ($80),y
                dey
                rts

byte            dta b(0)

                org $2000
text            equ *
                dta b(0),b(0)
                dta b(10) ; '\n'
                dta a(0)

                org $2E0
                dta a(start)

                end of file
