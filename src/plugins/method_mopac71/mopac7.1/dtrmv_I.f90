      MODULE dtrmv_I   
      INTERFACE
!...Generated by Pacific-Sierra Research 77to90  4.4G  22:48:56  03/08/06  
      SUBROUTINE dtrmv (UPLO, TRANS, DIAG, N, A, LDA, X, INCX) 
      USE vast_kind_param,ONLY: DOUBLE 
      character (LEN = *) :: UPLO 
      character (LEN = *) :: TRANS 
      character (LEN = *) :: DIAG 
      integer, INTENT(IN) :: N, LDA 
      real(DOUBLE), DIMENSION(LDA,*), INTENT(IN) :: A 
      real(DOUBLE), DIMENSION(*), INTENT(INOUT) :: X 
      integer, INTENT(IN) :: INCX 
      END SUBROUTINE  
      END INTERFACE 
      END MODULE 