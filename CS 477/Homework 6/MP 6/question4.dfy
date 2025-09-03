method Max( A:array<int>)
{
  var i: int := 0;
  var x: int := 0;
  assume A.Length > 0;
  x := A[0];
  i := 1;
  while (i < A.Length)
  // Write invariants
    invariant 0 <= i <= A.Length
    invariant forall j :: (0 <= j < i) ==> (A[j] <= x)
  {
    if (A[i] >= x)
        { x := A[i];}
    else
        { x := x;}
    i := i + 1;
  }
  assert forall j:: (0 <= j < A.Length) ==> (A[j] <= x);
}