## Summary of section 2 

- How to represent floating point in IEEE standard?
  - case 1: normalized case. 
    - the most common case. not all zeros, not all ones. 
    - Express the number as (-1) ^ s * M * 2 ^ E. 
      - where s is sign bit(the most leading.), M is 1 + f(the result of f part.), E is the result of e + (2^(k-1) + 1), 
      - Note that we the sign of E is different from sigh of the number itself( 2 ^ (-1)  vs  (-1) * 2)!
      - M is 1 + f because it is normalized format. 
  - case 2: Denormalized case. 
    - if all of number is zero, then we would interpet it as 0. 
    - M should be f itself(not add one)
    - if sign bit is zero, then it would be +0.0. if not so, it would be -0.0.
  - case 3: Special Values. 
    - all one case. it is interpreted as inf. 
    - According to sign bit, it can +inf or -inf. 


