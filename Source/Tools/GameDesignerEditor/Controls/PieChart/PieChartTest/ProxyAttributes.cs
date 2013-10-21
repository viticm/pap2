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
  /// <summary>
  /// Collects information about all of the attributes that apply to a method.  For
  /// property get and set methods, this includes attributes that are applied directly
  /// to the property and not to the get or set method.  This information governs
  /// how the proxy stub method is bound to instance methods of objects of different types.
  /// </summary>
  public class ProxyAttributeCollection
  {
    #region Constructor
    /// <summary>
    /// Constructs a new ProxyAttributeCollection.
    /// </summary>
    /// <param name="method">The method that is bound to this collection of attributes.</param>
    internal ProxyAttributeCollection(MethodInfo method)
    {
      this.method = method;
    }
    #endregion

    #region Fields
    /// <summary>
    /// The method bound to the collection of type bindings.
    /// </summary>
    private MethodInfo method;

    /// <summary>
    /// A dictionary that maps types to the <see cref="ProxyTypeBindingAttribute"/>s that govern how this method is
    /// bound to that type.
    /// </summary>
    private Dictionary<Type, ProxyTypeBindingAttribute> typeBindings = new Dictionary<Type, ProxyTypeBindingAttribute>();
    #endregion

    #region Properties
    /// <summary>
    /// Gets the method associated with this collection of attributes.
    /// </summary>
    public MethodInfo Method
    {
      get
      {
        return method;
      }
    }
    #endregion

    #region Methods
    /// <summary>
    /// Adds a binding that relates a type with a <see cref="ProxyTypeBindingAttribute"/>.
    /// </summary>
    /// <param name="attribute">The attributes that govern the invocation of methods on objects of a certain type.</param>
    public void AddTypeBinding(ProxyTypeBindingAttribute attribute)
    {
      try
      {
        this.typeBindings.Add(attribute.Type, attribute);
      }
      catch (ArgumentException)
      {
        throw new ProxyAttributeReflectionException();
      }
    }

    /// <summary>
    /// Gets the attribute that should be used for a given object instance.
    /// </summary>
    /// <param name="instance">The object instance that is going to be invoked by
    /// this method stub.</param>
    /// <returns>The attributes governing the invocation of methods on the instance object.</returns>
    /// <remarks>
    /// This method returns the best match attribute for the instance object.  For example, if the object
    /// instance is of type TextBox, and the list of bindings contains entries for the TextBoxBase and the
    /// Control classes, then the attributes for the TextBoxBase would be used for this object, since
    /// TextBoxBase is closer to TextBox in the inheritance hierarchy than Control.
    /// </remarks>
    public ProxyTypeBindingAttribute GetTypeBinding(object instance)
    {
      // get the instance of the type to get the attributes for
      Type instanceType = instance.GetType();

      // if the type is already here, return it
      if (typeBindings.ContainsKey(instanceType))
      {
        return typeBindings[instanceType];
      }

      // look for the correct type, which is the type in the list that is closest to the instance type in the inheritance hierarcy
      Type correctType = null;
      foreach (Type type in typeBindings.Keys)
      {
        if (type.IsAssignableFrom(instanceType) && (correctType == null || correctType.IsAssignableFrom(type)))
        {
          correctType = type;
        }
      }

      // if no types are in the inheritance hierarchy, use the instance type
      if (correctType == null)
      {
        correctType = instanceType;
      }

      // if the type isn't in the list, assume that the method/property/field has the same name
      if (!typeBindings.ContainsKey(correctType))
      {
        if (MemberInfoAccessor.IsGetMethod(Method) || MemberInfoAccessor.IsSetMethod(Method))
        {
          typeBindings.Add(correctType, new ProxyTypeBindingAttribute(correctType, MemberInfoAccessor.GetProperty(Method).Name));
        }
        else
        {
          typeBindings.Add(correctType, new ProxyTypeBindingAttribute(correctType, Method.Name));
        }
      }

      return typeBindings[correctType];
    }
    #endregion
  }

  /// <summary>
  /// Determines how a method or property stub will be invoked on remote objects.
  /// </summary>
  /// <remarks>
  /// By default, when a proxy stub is invoked and one of the proxy methods is called, each instance
  /// being proxied will be invoked with a method or property of the same name.  Use this attribute
  /// to redirect method or property invocation to a different name.  For all objects of the given type,
  /// the method, property, or field with the name targetName will be used.
  /// </remarks>
  [AttributeUsage(AttributeTargets.Property | AttributeTargets.Method, AllowMultiple = true)]
  public class ProxyTypeBindingAttribute : Attribute
  {
    #region Constructor
    /// <summary>
    /// Constructs a new ProxyTypeBindingAttribute.
    /// </summary>
    /// <param name="type">The type of objects this binding applies to.</param>
    /// <param name="targetName">The name of the method, field, or property of the type that should be bound to.</param>
    public ProxyTypeBindingAttribute(Type type, string targetName)
    {
      this.type = type;
      this.targetName = targetName;
    }
    #endregion

    #region Fields
    /// <summary>
    /// The type of object that this attribute governs.
    /// </summary>
    private Type type;

    /// <summary>
    /// The name of the method, field, or property on the type that will be invoked.
    /// </summary>
    private string targetName;
    #endregion

    #region Properties
    /// <summary>
    /// Gets the type of object that this attribute governs.
    /// </summary>
    public Type Type
    {
      get
      {
        return type;
      }
    }

    /// <summary>
    /// Gets the target name of the method, field, or property on the type that will be invoked.
    /// </summary>
    public string TargetName
    {
      get
      {
        return targetName;
      }
    }
    #endregion

    #region Methods
    /// <summary>
    /// Gets the accessor object that can be used to invoke the method, field, or property on the type of object with the
    /// given return value and arguments.
    /// </summary>
    /// <param name="returnType">The return type of the method, field, or property.</param>
    /// <param name="arguments">The arguments passed to the method, field, or property.</param>
    /// <returns>An object which can be used to invoke the member that matches the given name, return type,
    /// and arguments.
    /// </returns>
    public MemberInfoAccessor GetTargetAccessor(Type returnType, object[] arguments)
    {
      // get an array of members
      MemberInfo[] members = Type.GetMember(TargetName, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);

      // look for the member that will fit the caller
      foreach (MemberInfo member in members)
      {
        if (member.MemberType == MemberTypes.Property)
        {
          PropertyInfo propertyInfo = member as PropertyInfo;
          MethodInfo propertyGetMethod = propertyInfo.GetGetMethod(true);
          if (MemberInfoAccessor.VerifyMatch(propertyGetMethod, returnType, arguments))
          {
            return MemberInfoAccessor.BindMember(propertyGetMethod);
          }

          MethodInfo propertySetMethod = propertyInfo.GetSetMethod(true);
          if (MemberInfoAccessor.VerifyMatch(propertySetMethod, returnType, arguments))
          {
            return MemberInfoAccessor.BindMember(propertySetMethod);
          }
        }
        else if (member.MemberType == MemberTypes.Field)
        {
          FieldInfo fieldInfo = member as FieldInfo;
          if ((returnType == fieldInfo.FieldType && arguments.Length == 0))
          {
            return MemberInfoAccessor.BindMember(fieldInfo);
          }

          if (returnType == typeof(void) && arguments.Length == 1 && arguments[0].GetType().IsAssignableFrom(fieldInfo.FieldType))
          {
            return MemberInfoAccessor.BindMember(fieldInfo);
          }
        }
        else if (member.MemberType == MemberTypes.Method)
        {
          MethodInfo methodInfo = member as MethodInfo;
          if (MemberInfoAccessor.VerifyMatch(methodInfo, returnType, arguments))
          {
            return MemberInfoAccessor.BindMember(methodInfo);
          }
        }
      }

      throw new MissingProxyTargetException();
    }
    #endregion
  }
}