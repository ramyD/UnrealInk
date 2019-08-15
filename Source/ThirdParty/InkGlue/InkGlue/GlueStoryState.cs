using System;
using System.Collections.Generic;
using System.Text;
// using System.Text.RegularExpressions;
using Ink.Runtime;

namespace InkGlue
{
    public class GlueStoryState
    {
		internal GlueStoryState(StoryState inStoryState, Story parent)
		{
			_storyState = inStoryState;
            _story = parent;
		}

		public int InkSaveStateVersion()
		{
			return StoryState.kInkSaveStateVersion;
		}

		public string ToJson()
		{
			return _storyState.ToJson();
		}

		public void LoadJson(string json)
		{
			_storyState.LoadJson(json);
		}

        /// <summary>
        /// Loads the JSON state without overwriting global variables, visit counts, or turn indices
        /// </summary>
        /// <param name="json"></param>
        public void LoadJsonButMaintainGlobalState(string json)
        {
            _storyState.LoadJsonIgnoreGlobalState(json);
        }

		public int VisitCountAtPathString(string pathString)
		{
			return _storyState.VisitCountAtPathString(pathString);
		}

		public void ForceEnd()
		{
			_storyState.ForceEnd();
		}

		StoryState _storyState;
        Story _story;
    }
}
