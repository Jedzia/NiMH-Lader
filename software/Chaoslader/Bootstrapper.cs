//  --------------------------------------------------------------------------------------------------------------------
//  <copyright file="Bootstrapper.cs" company="EvePanix">Copyright (c) Jedzia 2001-2014, EvePanix. All rights reserved. See the license notes shipped with this source and the GNU GPL.</copyright>
//  <author>Jedzia</author>
//  <email>jed69@gmx.de</email>
//  <date>10.01.2015 16:55</date>
//  --------------------------------------------------------------------------------------------------------------------

namespace Accelknife
{
    using System;

    using Castle.Facilities.Logging;
    using Castle.Facilities.TypedFactory;
    using Castle.MicroKernel.Registration;
    using Castle.MicroKernel.Resolvers.SpecializedResolvers;
    using Castle.Windsor;
    using Castle.Windsor.Installer;

    /// <summary>
    ///     The startup bootstrapper.
    /// </summary>
    public class Bootstrapper : IDisposable
    {
        /// <summary>
        ///     TODO The container.
        /// </summary>
        private IWindsorContainer container;

        /// <summary>
        ///     Initializes a new instance of the <see cref="Bootstrapper" /> class.
        /// </summary>
        public Bootstrapper()
        {
            try
            {
                this.container = new WindsorContainer();
                this.container.Kernel.Resolver.AddSubResolver(new CollectionResolver(this.container.Kernel, false));
                this.container.AddFacility<TypedFactoryFacility>();
                //this.container.AddFacility<LoggingFacility>(f => f.UseNLog());
                this.container.AddFacility<LoggingFacility>(f => f.LogUsing(LoggerImplementation.NLog)
                                                         .WithConfig("NLog.config"));
                // Types.FromAssemblyInThisApplication() ?
                //this.container.Install(FromAssembly.Containing<Bootstrapper>());
                //this.container.Install(FromAssembly.Named("Accelknife.Blades.Overview"));
                //this.container.Install(FromAssembly.InThisApplication());

                //this.container.Install(FromAssembly.InDirectory(new AssemblyFilter(Path.GetDirectoryName(Application.ExecutablePath))));
                this.container.Install(FromAssembly.InDirectory(new AssemblyFilter("", "Accelknife*")));

                //var eee = container.Resolve<IBladeTest>();
                //var encoderFactory = container.Resolve<IEncoderFactory>();
                //var encoder = encoderFactory.GetByName("Accelknife.Blades.RecodeMule.Encoding.FFmpeg");
                //var encoder = encoderFactory.GetByName("Mencoder");
            }
            catch (Exception ex)
            {
                //MessageBox.Show(ex.ToString());
            }
        }

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        ///     Gets the main application object.
        /// </summary>
        /// <returns>
        ///     the Windows Form <see cref="Form1" /> .
        /// </returns>
        public IWindsorContainer Run()
        {
            //return this.container.Resolve<Form1>();
            return this.container;
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // free managed resources
                if (this.container != null)
                {
                    this.container.Dispose();
                    this.container = null;
                }
            }
            // free native resources if there are any.
            /*if (nativeResource != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(nativeResource);
                nativeResource = IntPtr.Zero;
            }*/
        }
    }
}