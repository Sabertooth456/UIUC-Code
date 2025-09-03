method m(n: nat)
{
  var i: int := 0;
  while i < n  
    // Loop invariant
    invariant i <= n
  {
    i := i + 1;
  }
  assert i == n;
}