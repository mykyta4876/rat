using Quasar.Server.Forms;
using Quasar.Server.ViewModel;
using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Net;
using System.Text;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.ListView;


namespace Quasar.Server
{
    internal static class Program
    {
        [STAThread]
        private static void Main()
        {
            // enable TLS 1.2
            ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12;
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Application.Run(new FrmMain());
        }

    }
}