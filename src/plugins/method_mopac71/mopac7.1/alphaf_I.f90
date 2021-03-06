      MODULE alphaf_I   
      INTERFACE
!...Generated by Pacific-Sierra Research 77to90  4.4G  21:23:38  03/14/06  
      SUBROUTINE alphaf (IWFLA, ATOL, MAXITA, U, F, G, UOLD, H1, D, DA, W2, C, W) 
      USE vast_kind_param,ONLY: DOUBLE 
      use molkst_C, only : norbs
      integer, INTENT(IN) :: IWFLA 
      real(DOUBLE) :: ATOL 
      integer, INTENT(IN) :: MAXITA 
      real(DOUBLE), DIMENSION(norbs,norbs) :: U 
      real(DOUBLE), DIMENSION(norbs,norbs) :: F 
      real(DOUBLE), DIMENSION(norbs,norbs) :: G 
      real(DOUBLE), DIMENSION(norbs,norbs) :: UOLD 
      real(DOUBLE), DIMENSION(norbs,norbs) :: H1 
      real(DOUBLE), DIMENSION(norbs,norbs) :: D 
      real(DOUBLE), DIMENSION(norbs,norbs) :: DA 
      real(DOUBLE), DIMENSION(*), intent(out) :: W2 
      real(DOUBLE), DIMENSION(*), intent(in) :: C 
      real(DOUBLE), DIMENSION(*), intent(in) :: W 
      END SUBROUTINE  
      END INTERFACE 
      END MODULE 
