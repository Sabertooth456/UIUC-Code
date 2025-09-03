method m(n: nat)
{
  var i: int := 0;
  var ans: int := 0;
  while i < n 
  // Write invariants
    invariant i <= n
    invariant ans == i * (i + 1)
  {
    i := i + 1;
    ans := ans + 2*i;
  }
  assert ans == n*(n+1);
}