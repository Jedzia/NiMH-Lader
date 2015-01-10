using System.Windows;
using GalaSoft.MvvmLight.Threading;

namespace Jedzia.Arduino.Chaoslader.Application
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        static App()
        {
            DispatcherHelper.Initialize();
        }
    }
}
