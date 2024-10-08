﻿using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

using CLI;

using System.Diagnostics;
using System.Text.RegularExpressions;
using static System.Runtime.InteropServices.JavaScript.JSType;
using System.Reflection.PortableExecutable;

namespace ZippGUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        SolverManager manager;
        public MainWindow()
        {
            manager = new SolverManager();
            InitializeComponent();
            // Set default values for the inputs
            numOfStages.Text = "4";
            numOfTasks.Text = "10";
            maxNumOfMachinesInStage.Text = "5";
            maxTaskTime.Text = "20";
        }

        private void GenerateButtonClick(object sender, RoutedEventArgs e)
        {
            try
            {
                int s = int.Parse(numOfStages.Text);
                int n = int.Parse(numOfTasks.Text);
                int maxMIS = int.Parse(maxNumOfMachinesInStage.Text);
                int maxTT = int.Parse(maxTaskTime.Text);
                Tuple<int[], int[,]> _ = manager.generate(s, n, maxMIS, maxTT);
                GeneratedText.Text = tuple2string(_);
                // TODO save it and display nicely
            }
            catch (Exception ex)
            {
                // TODO Exception handling
            }
        }

        private void NumberValidationTextBox(object sender, TextCompositionEventArgs e)
        {
            Regex regex = new Regex("^[0-9]+$");
            e.Handled = !regex.IsMatch(e.Text);
        }

        private void AlgorithmChoice_Loaded(object sender, EventArgs e)
        {
            string[] algorithmNames = manager.getAlgorithmNames();

            AlgorithmChoice.ItemsSource = algorithmNames;
            if (algorithmNames.Length > 0)
            {
                AlgorithmChoice.SelectedIndex = 0;
            }
        }

        private void Run_Button_Click(object sender, RoutedEventArgs e)
        {
            int algId = AlgorithmChoice.SelectedIndex;
            //manager.run(algId, ...);
        }

        private string tuple2string(Tuple<int[], int[,]> instance)
        {
            // Extract machines and taskTimes from the Tuple
            int[] machines = instance.Item1;
            int[,] taskTimes = instance.Item2;

            // Initialize result string
            StringBuilder str = new StringBuilder();
            str.AppendLine("Instance object:");
            str.AppendLine("Stages:");

            // Convert machines (1D array) to string
            str.Append("[");
            for (int i = 0; i < machines.Length - 1; i++)
            {
                str.Append(machines[i].ToString() + ", ");
            }
            str.Append(machines[machines.Length - 1].ToString() + "]\n");

            // Convert taskTimes (2D array) to string
            str.AppendLine("Tasks:");
            for (int i = 0; i < taskTimes.GetLength(0); i++)
            {
                str.Append("[");
                for (int j = 0; j < taskTimes.GetLength(1) - 1; j++)
                {
                    str.Append(taskTimes[i, j].ToString() + ", ");
                }
                str.Append(taskTimes[i, taskTimes.GetLength(1) - 1].ToString() + "]\n");
            }

            return str.ToString();
        }

        private void PopulateAlgorithmParams(int algId)
        {
            // Get the algorithm parameters from the getAlgorithmParams method
            Dictionary<string, int> parameters = manager.getAlgorithmParams(algId);

            // Clear existing children in the AlgorithmParamsPanel
            AlgorithmParamsPanel.Children.Clear();

            foreach (var param in parameters)
            {
                // Create a StackPanel to hold the label and textbox
                StackPanel paramPanel = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(0, 5, 0, 5) };

                // Create the label for the parameter
                Label label = new Label
                {
                    Content = param.Key,
                    Width = 150,
                    VerticalAlignment = VerticalAlignment.Center
                };

                // Create the TextBox for the parameter value
                TextBox textBox = new TextBox
                {
                    Width = 100,
                    Text = param.Value.ToString(),
                    Background = System.Windows.Media.Brushes.White,
                    BorderBrush = System.Windows.Media.Brushes.Gray,
                    Padding = new Thickness(5),
                    Margin = new Thickness(10, 0, 0, 0)
                };

                // Add the label and textbox to the paramPanel
                paramPanel.Children.Add(label);
                paramPanel.Children.Add(textBox);

                // Add the paramPanel to the AlgorithmParamsPanel
                AlgorithmParamsPanel.Children.Add(paramPanel);
            }
        }

        private void AlgorithmChoice_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PopulateAlgorithmParams(AlgorithmChoice.SelectedIndex);
        }


        private void SetParametersButtonClick(object sender, RoutedEventArgs e)
        {
            // TODO Setting parameters
        }
    }
}