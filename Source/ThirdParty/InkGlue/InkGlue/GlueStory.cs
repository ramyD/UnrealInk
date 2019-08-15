using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Ink.Runtime;

namespace InkGlue
{
	public class GlueStory
	{
		[DllImport("__Internal")]
		public static extern void ObserverCallbackInt(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, int newValue);

		[DllImport("__Internal")]
		public static extern void ObserverCallbackFloat(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, float newValue);

		[DllImport("__Internal")]
		public static extern void ObserverCallbackString(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, string newValue);

        public enum InkVarType
        {
            Float,
            Int,
            String,
            None
        }

        // DO NOT MAKE CHANGES TO THIS STRUCTURE WITHOUT CHANGING Story.h::FInkVarInterop in C++
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct InkVarInterop
        {
            [MarshalAs(UnmanagedType.U1)]
            public InkVarType Type;

            [MarshalAs(UnmanagedType.R4)]
            public float FloatVal;

            [MarshalAs(UnmanagedType.I4)]
            public int IntVal;

            [MarshalAs(UnmanagedType.LPStr)]
            public string StringVal;

            public InkVarInterop(object arg)
            {
                FloatVal = 0; IntVal = 0; StringVal = null;

                if (arg is float)
                {
                    FloatVal = (float)arg;
                    Type = InkVarType.Float;
                }
                else if (arg is int)
                {
                    IntVal = (int)arg;
                    Type = InkVarType.Int;
                }
                else if (arg is string)
                {
                    StringVal = (string)arg;
                    Type = InkVarType.String;
                }
                else
                {
                    throw new Exception("Invalid Ink Variable Type: " + arg.GetType());
                }
            }

            public object BoxedValue
            {
                get
                {
                    switch(Type)
                    {
                        case InkVarType.Float:
                            return FloatVal;
                        case InkVarType.Int:
                            return IntVal;
                        case InkVarType.String:
                            return StringVal;
                        default:
                        case InkVarType.None:
                            return null;
                    }
                }
            }
        }

        [DllImport("__Internal")]
        public static extern InkVarInterop ExternalFunctionCallback(
            [MarshalAs(UnmanagedType.I4)] int instanceId,
            [MarshalAs(UnmanagedType.LPStr)] string functionName, 
            [MarshalAs(UnmanagedType.U4)] uint numArguments,
            [MarshalAs(UnmanagedType.LPArray)] InkVarInterop[] arguments);

		public GlueStory(string JsonString, int instanceId)
		{
			_story = new Story(JsonString);
			_instanceId = instanceId;
		}

		public bool CanContinue()
		{
			return _story.canContinue;
		}

		public string Continue()
		{
			return _story.Continue();
		}

		public GlueChoice[] CurrentChoices()
		{
			GlueChoice[] glueChoices = new GlueChoice[_story.currentChoices.Count];
			for (int i = 0; i < _story.currentChoices.Count; i++)
			{
				glueChoices[i] = new GlueChoice(_story.currentChoices[i]);
			}
			return glueChoices;
		}

		public void ChooseChoiceIndex(int index)
		{
			_story.ChooseChoiceIndex(index);
		}

		public string CurrentText()
		{
			return _story.currentText;
		}

		public GlueStoryState State()
		{
			return new GlueStoryState(_story.state, _story);
		}

        public GlueVariablesState VariablesState()
        {
            return new GlueVariablesState(_story.variablesState);
        }

		public bool HasError()
		{
			return _story.hasError;
		}

		public string[] CurrentTags()
		{
			return _story.currentTags.ToArray();
		}

		public string[] CurrentErrors()
		{
			return _story.currentErrors.ToArray();
		}

		public void ResetState()
		{
			_story.ResetState();
		}

		public void ResetErrors()
		{
			_story.ResetErrors();
		}

		public void ResetCallstack()
		{
			_story.ResetCallstack();
		}

		public void ContinueAsync(float millisecondLimitAsync)
		{
			_story.ContinueAsync(millisecondLimitAsync);
		}

		public string ContinueMaximally()
		{
			return _story.ContinueMaximally();
		}

		public bool AsyncContinueComplete()
		{
			return _story.asyncContinueComplete;
		}

		public string[] TagsForContentAtPath(string path)
		{
			List<string> tags = _story.TagsForContentAtPath(path);
			if (tags == null)
				return new string[0];
			else
				return tags.ToArray();
		}

		public void ChoosePathString(string path, object[] arguments, bool resetCallStack = true)
		{
			_story.ChoosePathString(path, resetCallStack, arguments);
		}

        object FunctionImplInternal(string functionName, object[] args)
        {
            // Convert arguments into Interop Variable structs
            InkVarInterop[] cArgs = new InkVarInterop[args.Length];
            for (int i = 0; i < args.Length; i++)
            {
                cArgs[i] = new InkVarInterop(args[i]);
            }

            // Call the callback
            InkVarInterop var = ExternalFunctionCallback(_instanceId, functionName, (uint)cArgs.Length, cArgs);

            // Box the return value and send it back to the ink runtime
            return var.BoxedValue;
        }

        public void RegisterFunction(string functionName)
        {
            // Bind to internal C# method
            _story.BindExternalFunctionGeneral(functionName, args => FunctionImplInternal(functionName, args));
        }

        public void UnregisterFunction(string functionName)
        {
            // Unbind
            _story.UnbindExternalFunction(functionName);
        }

		void InternalObserve(string variableName, object newValue)
		{
			if (newValue is int) {
				ObserverCallbackInt(_instanceId, variableName, (int)newValue);
			} else if (newValue is float){
				ObserverCallbackFloat(_instanceId, variableName, (float)newValue);
			} else if (newValue is string){
				ObserverCallbackString(_instanceId, variableName, (string)newValue);
			}
		}

		public void ObserveVariable(string variableName)
		{
			_story.ObserveVariable(variableName, InternalObserve);
		}

		public void RemoveVariableObserver(string variableName)
		{
			_story.RemoveVariableObserver(InternalObserve, variableName);
		}

		Story _story;
		int _instanceId = -1;
	}
}
