using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;
using System.Configuration;

using GaDotNet.Common;
using GaDotNet.Common.Data;
using GaDotNet.Common.Helpers;

namespace AnalyticsService
{
    /// <summary>
    /// Summary description for track
    /// </summary>
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.ComponentModel.ToolboxItem(false)]
    [System.Web.Script.Services.ScriptService]
    public class track : System.Web.Services.WebService
    {

        [WebMethod]
        public void SendGAEvent(string category, string action, string label)
        {
            string domain = ConfigurationManager.AppSettings.Get("GADomain");
            if (string.Compare(category, "testpageview") == 0)
            {
                GooglePageView eventToTrack = new GooglePageView("my test",
                  domain, "/test.aspx");

                TrackingRequest req = new RequestFactory().BuildRequest(eventToTrack);
                GoogleTracking.FireTrackingEvent(req);
            }

            if (!string.IsNullOrEmpty(category) &&
                !string.IsNullOrEmpty(action) &&
                !string.IsNullOrEmpty(label))
            {
                GoogleEvent eventToTrack = new GoogleEvent(domain, category, action, label, 0);

                TrackingRequest req = new RequestFactory().BuildRequest(eventToTrack);
                GoogleTracking.FireTrackingEvent(req);
            }
        }
    }
}
