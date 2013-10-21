/// Author:  Matthew Johnson
/// Version: 1.0
/// Date:    March 13, 2006
/// Notice:  You are free to use this code as you wish.  There are no guarantees whatsoever about
/// its usability or fitness of purpose.

#region using
using System;
using System.Collections.Generic;
using System.Reflection;
#endregion

namespace Nexus.Reflection
{
  public abstract class MemberInfoAccessor
  {
    #region Factory Methods
    public static MemberInfoAccessor BindMember(MemberInfo member)
    {
      if (member is FieldInfo)
      {
        return new FieldInfoAccessor(member as FieldInfo);
      }
      else if (member is PropertyInfo)
      {
        return new PropertyInfoAccessor(member as PropertyInfo);
      }
      else if (member is MethodInfo)
      {
        return new MethodInfoAccessor(member as MethodInfo);
      }

      throw new Exception();
    }
    #endregion

    #region Fields
    private const string GetPrefix = "get_";
    private const string SetPrefix = "set_";
    #endregion

    #region Methods
    public abstract MemberInfo MemberInfo { get; }
    public abstract object Get(object target, params object[] args);
    public abstract void Set(object target, object value, params object[] args);

    internal static bool IsGetMethod(MethodInfo method)
    {
      return method.IsSpecialName && method.Name.StartsWith(GetPrefix);
    }

    internal static bool IsSetMethod(MethodInfo method)
    {
      return method.IsSpecialName && method.Name.StartsWith(SetPrefix);
    }

    internal static string GetPropertyName(MethodInfo method)
    {
      return method.Name.Substring(GetPrefix.Length);
    }

    internal static PropertyInfo GetProperty(MethodInfo method)
    {
      if (IsGetMethod(method))
      {
        return method.ReflectedType.GetProperty(GetPropertyName(method), BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
      }
      else if (IsSetMethod(method))
      {
        return method.ReflectedType.GetProperty(GetPropertyName(method), BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
      }

      return null;
    }

    //internal static bool VerifyMatch(MethodInfo methodBase, MethodInfo methodInvoke)
    //{
    //  // check the return values
    //  if (methodBase.ReturnType != methodInvoke.ReturnType)
    //  {
    //    return false;
    //  }

    //  ParameterInfo[] baseParameters = methodBase.GetParameters();
    //  ParameterInfo[] invokeParameters = methodInvoke.GetParameters();

    //  if (baseParameters.Length != invokeParameters.Length)
    //  {
    //    return false;
    //  }

    //  for (int i = 0; i < baseParameters.Length; i++)
    //  {
    //    if (baseParameters[i].ParameterType != invokeParameters[i].ParameterType)
    //    {
    //      return false;
    //    }
    //  }

    //  return true;
    //}

    internal static bool VerifyMatch(MethodInfo method, Type returnType, object[] args)
    {
      if (method.ReturnType != returnType)
      {
        return false;
      }

      ParameterInfo[] parameters = method.GetParameters();

      if (parameters.Length != args.Length)
      {
        return false;
      }

      for (int i = 0; i < parameters.Length; i++)
      {
        if (!(parameters[i].ParameterType.IsAssignableFrom(args[i].GetType())))
        {
          return false;
        }
      }

      return true;
    }
    #endregion

    #region class FieldInfoAccessor
    private class FieldInfoAccessor : MemberInfoAccessor
    {
      public FieldInfoAccessor(FieldInfo field)
      {
        this.field = field;
      }

      private FieldInfo field;

      public override MemberInfo MemberInfo
      {
        get
        {
          return field;
        }
      }

      public override object Get(object target, params object[] args)
      {
        if (args.Length > 0)
        {
          throw new Exception(); //todo: better exception
        }

        return field.GetValue(target);
      }

      public override void Set(object target, object value, params object[] args)
      {
        if (args.Length > 0)
        {
          throw new Exception(); //todo: better exception
        }

        field.SetValue(target, value);
      }
    }
    #endregion

    #region class PropertyInfoAccessor
    private class PropertyInfoAccessor : MemberInfoAccessor
    {
      public PropertyInfoAccessor(PropertyInfo property)
      {
        this.property = property;
      }

      private PropertyInfo property;

      public override MemberInfo MemberInfo
      {
        get
        {
          return property;
        }
      }

      public override object Get(object target, params object[] args)
      {
        try
        {
          return property.GetValue(target, args);
        }
        catch
        {
          throw new Exception(); //todo
        }
      }

      public override void Set(object target, object value, params object[] args)
      {
        try
        {
          property.SetValue(target, value, args);
        }
        catch
        {
          throw new Exception(); //todo
        }
      }
    }
    #endregion

    #region class MethodInfoAccessor
    private class MethodInfoAccessor : MemberInfoAccessor
    {
      public MethodInfoAccessor(MethodInfo method)
      {
        this.method = method;
      }

      private MethodInfo method;

      public override MemberInfo MemberInfo
      {
        get
        {
          return method;
        }
      }

      public override object Get(object target, params object[] args)
      {
        try
        {
          return method.Invoke(target, args);
        }
        catch
        {
          throw new Exception();
        }
      }

      public override void Set(object target, object value, params object[] args)
      {
        try
        {
          object[] invoke = new object[args.Length + 1];
          invoke[0] = value;
          Array.Copy(args, 0, invoke, 1, args.Length);
          method.Invoke(target, invoke);
        }
        catch
        {
          throw new Exception();
        }
      }
    }
    #endregion
  }
}