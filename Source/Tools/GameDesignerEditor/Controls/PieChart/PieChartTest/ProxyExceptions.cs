/// Author:  Matthew Johnson
/// Version: 1.0
/// Date:    March 13, 2006
/// Notice:  You are free to use this code as you wish.  There are no guarantees whatsoever about
/// its usability or fitness of purpose.

#region using
using System;
#endregion

namespace Nexus.Reflection
{
  public class ProxyException : Exception
  {
  }

  public class ProxyAttributeReflectionException : ProxyException
  {
  }

  public class MissingProxyTargetException : ProxyException
  {
  }
}