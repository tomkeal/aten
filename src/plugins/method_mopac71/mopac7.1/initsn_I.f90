      MODULE initsn_I   
      INTERFACE
!...Generated by Pacific-Sierra Research 77to90  4.4G  10:47:04  03/09/06  
      SUBROUTINE initsn (INDEPS, DIRSM, DIRSMH) 
      USE vast_kind_param,ONLY: DOUBLE 
      use cosmo_C, only : nppa 
      integer :: INDEPS 
      real(DOUBLE), DIMENSION(3,NPPA) :: DIRSM 
      real(DOUBLE), DIMENSION(3,NPPA/3) :: DIRSMH 
      END SUBROUTINE  
      END INTERFACE 
      END MODULE 
