/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:Jedzia.Arduino.Chaoslader.Application.ViewModel"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
*/

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Ioc;
using Microsoft.Practices.ServiceLocation;
using Jedzia.Arduino.Chaoslader.Application.Model;

namespace Jedzia.Arduino.Chaoslader.Application.ViewModel
{
    using Accelknife;

    using Castle.Core.Logging;
    using Castle.MicroKernel.Registration;
    using Castle.Windsor;

    /// <summary>
    /// This class contains static references to all the view models in the
    /// application and provides an entry point for the bindings.
    /// <para>
    /// See http://www.galasoft.ch/mvvm
    /// </para>
    /// </summary>
    public class ViewModelLocator
    {
        private static readonly IWindsorContainer container;

        static ViewModelLocator()
        {
            container = new Bootstrapper().Run();
            //ServiceLocator.SetLocatorProvider(() => SimpleIoc.Default);

            if (ViewModelBase.IsInDesignModeStatic)
            {
                //SimpleIoc.Default.Register<IDataService, Design.DesignDataService>();
                container.Register(Component.For<IDataService>().ImplementedBy<Design.DesignDataService>());
            }
            else
            {
                //SimpleIoc.Default.Register<IDataService, DataService>();
                container.Register(Component.For<IDataService>().ImplementedBy<DataService>());
            }

            //SimpleIoc.Default.Register<MainViewModel>();
            container.Register(Component.For<MainViewModel>());
        }

        /// <summary>
        /// Gets the Main property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public MainViewModel Main
        {
            get
            {
                //return ServiceLocator.Current.GetInstance<MainViewModel>();
                return container.Resolve<MainViewModel>();
            }
        }

        public static ILogger GetLogger()
        {
            return container.Resolve<ILogger>();
        }

        /// <summary>
        /// Cleans up all the resources.
        /// </summary>
        public static void Cleanup()
        {
        }
    }
}