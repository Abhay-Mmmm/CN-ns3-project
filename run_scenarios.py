#!/usr/bin/env python3
"""
Football Network Simulation Scenario Runner

This script runs different simulation scenarios defined in the configuration file
and compares their performance.
"""

import configparser
import subprocess
import os
import sys
import time
from typing import Dict, List
import argparse

class SimulationRunner:
    def __init__(self, config_file: str = "simulation_config.ini"):
        self.config_file = config_file
        self.config = configparser.ConfigParser()
        self.results_dir = "scenario_results"
        
        # Load configuration
        if os.path.exists(config_file):
            self.config.read(config_file)
        else:
            print(f"Warning: Configuration file {config_file} not found.")
            print("Using default parameters only.")
    
    def get_scenarios(self) -> List[str]:
        """Get list of available scenarios"""
        scenarios = [section for section in self.config.sections() 
                    if section.startswith('scenario_')]
        return scenarios
    
    def run_scenario(self, scenario_name: str) -> bool:
        """Run a specific simulation scenario"""
        if scenario_name not in self.config.sections():
            print(f"Error: Scenario '{scenario_name}' not found in configuration.")
            return False
        
        scenario = self.config[scenario_name]
        
        # Extract parameters
        simulation_time = scenario.get('simulation_time', '10.0')
        image_size = scenario.get('image_size', '50000')
        packet_size = scenario.get('packet_size', '1024')
        data_rate = scenario.get('data_rate', '1Mbps')
        delay = scenario.get('delay', '2ms')
        enable_netanim = scenario.get('enable_netanim', 'true').lower() == 'true'
        description = scenario.get('description', 'No description')
        
        print(f"\n{'='*60}")
        print(f"Running Scenario: {scenario_name}")
        print(f"Description: {description}")
        print(f"{'='*60}")
        print(f"Parameters:")
        print(f"  Simulation Time: {simulation_time}s")
        print(f"  Image Size: {image_size} bytes")
        print(f"  Packet Size: {packet_size} bytes")
        print(f"  Data Rate: {data_rate}")
        print(f"  Delay: {delay}")
        print(f"  NetAnim: {enable_netanim}")
        print()
        
        # Create scenario-specific results directory
        scenario_dir = os.path.join(self.results_dir, scenario_name)
        os.makedirs(scenario_dir, exist_ok=True)
        
        # Build NS-3 simulation command
        cmd = [
            "../ns3", "run",
            f"CN-ns3-project/footballer-network-sim",
            "--",
            f"--simulationTime={simulation_time}",
            f"--imageSize={image_size}",
            f"--packetSize={packet_size}",
            f"--dataRate={data_rate}",
            f"--delay={delay}",
            f"--enableNetAnim={str(enable_netanim).lower()}"
        ]
        
        print("Executing command:")
        print(" ".join(cmd))
        print()
        
        # Change to NS-3 root directory
        original_dir = os.getcwd()
        try:
            os.chdir("..")
            
            # Run the simulation
            start_time = time.time()
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=300)
            end_time = time.time()
            
            execution_time = end_time - start_time
            
            if result.returncode == 0:
                print(f"✓ Scenario completed successfully in {execution_time:.2f} seconds")
                
                # Save simulation output
                output_file = os.path.join(scenario_dir, "simulation_output.txt")
                with open(output_file, 'w') as f:
                    f.write(f"Scenario: {scenario_name}\n")
                    f.write(f"Description: {description}\n")
                    f.write(f"Execution Time: {execution_time:.2f} seconds\n")
                    f.write(f"Return Code: {result.returncode}\n\n")
                    f.write("STDOUT:\n")
                    f.write(result.stdout)
                    f.write("\nSTDERR:\n")
                    f.write(result.stderr)
                
                # Move generated files to scenario directory
                files_to_move = [
                    "footballer-network-anim.xml",
                    "flowmon-results.xml"
                ]
                
                for filename in files_to_move:
                    if os.path.exists(filename):
                        os.rename(filename, os.path.join(scenario_dir, filename))
                
                return True
            else:
                print(f"✗ Scenario failed with return code {result.returncode}")
                print("Error output:")
                print(result.stderr)
                
                # Save error information
                error_file = os.path.join(scenario_dir, "error_output.txt")
                with open(error_file, 'w') as f:
                    f.write(f"Scenario: {scenario_name}\n")
                    f.write(f"Return Code: {result.returncode}\n")
                    f.write(f"Execution Time: {execution_time:.2f} seconds\n\n")
                    f.write("STDOUT:\n")
                    f.write(result.stdout)
                    f.write("\nSTDERR:\n")
                    f.write(result.stderr)
                
                return False
                
        except subprocess.TimeoutExpired:
            print(f"✗ Scenario timed out after 300 seconds")
            return False
        except Exception as e:
            print(f"✗ Error running scenario: {e}")
            return False
        finally:
            os.chdir(original_dir)
    
    def analyze_scenario_results(self, scenario_name: str) -> bool:
        """Analyze results for a specific scenario"""
        scenario_dir = os.path.join(self.results_dir, scenario_name)
        flowmon_file = os.path.join(scenario_dir, "flowmon-results.xml")
        
        if not os.path.exists(flowmon_file):
            print(f"Warning: No FlowMonitor results found for {scenario_name}")
            return False
        
        print(f"Analyzing results for {scenario_name}...")
        
        # Get scenario parameters for analysis
        scenario_config = self.config[scenario_name] if scenario_name in self.config.sections() else {}
        simulation_time = float(scenario_config.get('simulation_time', '10.0'))
        image_size = int(scenario_config.get('image_size', '50000'))
        
        # Run analysis script
        analysis_cmd = [
            sys.executable, "analyze_results.py",
            "--flowmon", flowmon_file,
            "--output", scenario_dir,
            "--simulation-time", str(simulation_time),
            "--image-size", str(image_size),
            "--no-plots"  # Don't show plots for batch processing
        ]
        
        try:
            result = subprocess.run(analysis_cmd, capture_output=True, text=True)
            if result.returncode == 0:
                print(f"✓ Analysis completed for {scenario_name}")
                return True
            else:
                print(f"✗ Analysis failed for {scenario_name}")
                print(result.stderr)
                return False
        except Exception as e:
            print(f"✗ Error analyzing {scenario_name}: {e}")
            return False
    
    def run_all_scenarios(self) -> Dict[str, bool]:
        """Run all available scenarios"""
        scenarios = self.get_scenarios()
        results = {}
        
        if not scenarios:
            print("No scenarios found in configuration file.")
            return results
        
        print(f"Found {len(scenarios)} scenarios to run:")
        for scenario in scenarios:
            description = self.config[scenario].get('description', 'No description')
            print(f"  - {scenario}: {description}")
        
        print(f"\nResults will be saved to: {self.results_dir}")
        os.makedirs(self.results_dir, exist_ok=True)
        
        # Run each scenario
        successful_scenarios = []
        failed_scenarios = []
        
        for i, scenario in enumerate(scenarios, 1):
            print(f"\n[{i}/{len(scenarios)}] Processing {scenario}...")
            
            if self.run_scenario(scenario):
                successful_scenarios.append(scenario)
                results[scenario] = True
                
                # Analyze results
                self.analyze_scenario_results(scenario)
            else:
                failed_scenarios.append(scenario)
                results[scenario] = False
        
        # Print summary
        print(f"\n{'='*60}")
        print("SIMULATION SUMMARY")
        print(f"{'='*60}")
        print(f"Total Scenarios: {len(scenarios)}")
        print(f"Successful: {len(successful_scenarios)}")
        print(f"Failed: {len(failed_scenarios)}")
        
        if successful_scenarios:
            print(f"\nSuccessful scenarios:")
            for scenario in successful_scenarios:
                print(f"  ✓ {scenario}")
        
        if failed_scenarios:
            print(f"\nFailed scenarios:")
            for scenario in failed_scenarios:
                print(f"  ✗ {scenario}")
        
        return results
    
    def compare_scenarios(self, scenarios: List[str] = None):
        """Generate a comparison report across scenarios"""
        if scenarios is None:
            scenarios = [s for s in self.get_scenarios() if os.path.exists(
                os.path.join(self.results_dir, s, "simulation_results.csv"))]
        
        if not scenarios:
            print("No scenarios with results found for comparison.")
            return
        
        print(f"\nGenerating comparison report for {len(scenarios)} scenarios...")
        
        # This would implement cross-scenario comparison
        # For now, we'll create a simple summary
        comparison_file = os.path.join(self.results_dir, "scenario_comparison.txt")
        
        with open(comparison_file, 'w') as f:
            f.write("FOOTBALL NETWORK SIMULATION - SCENARIO COMPARISON\n")
            f.write("=" * 60 + "\n\n")
            
            for scenario in scenarios:
                scenario_config = self.config[scenario] if scenario in self.config.sections() else {}
                description = scenario_config.get('description', 'No description')
                
                f.write(f"Scenario: {scenario}\n")
                f.write(f"Description: {description}\n")
                f.write(f"Data Rate: {scenario_config.get('data_rate', 'N/A')}\n")
                f.write(f"Delay: {scenario_config.get('delay', 'N/A')}\n")
                f.write(f"Image Size: {scenario_config.get('image_size', 'N/A')} bytes\n")
                f.write("-" * 40 + "\n")
        
        print(f"Comparison report saved to {comparison_file}")

def main():
    parser = argparse.ArgumentParser(description='Run Football Network Simulation Scenarios')
    parser.add_argument('--config', default='simulation_config.ini',
                       help='Configuration file with scenarios')
    parser.add_argument('--scenario', help='Run specific scenario only')
    parser.add_argument('--list', action='store_true',
                       help='List available scenarios')
    parser.add_argument('--analyze-only', help='Only analyze existing results for scenario')
    parser.add_argument('--compare', action='store_true',
                       help='Generate comparison report')
    
    args = parser.parse_args()
    
    # Create runner instance
    runner = SimulationRunner(args.config)
    
    # List scenarios
    if args.list:
        scenarios = runner.get_scenarios()
        print("Available scenarios:")
        for scenario in scenarios:
            description = runner.config[scenario].get('description', 'No description')
            print(f"  {scenario}: {description}")
        return 0
    
    # Analyze only
    if args.analyze_only:
        if runner.analyze_scenario_results(args.analyze_only):
            return 0
        else:
            return 1
    
    # Compare scenarios
    if args.compare:
        runner.compare_scenarios()
        return 0
    
    # Run specific scenario
    if args.scenario:
        if runner.run_scenario(args.scenario):
            runner.analyze_scenario_results(args.scenario)
            return 0
        else:
            return 1
    
    # Run all scenarios
    results = runner.run_all_scenarios()
    
    # Generate comparison if any scenarios succeeded
    successful_scenarios = [s for s, success in results.items() if success]
    if successful_scenarios:
        runner.compare_scenarios(successful_scenarios)
    
    # Return appropriate exit code
    if any(results.values()):
        return 0
    else:
        return 1

if __name__ == "__main__":
    exit(main())
