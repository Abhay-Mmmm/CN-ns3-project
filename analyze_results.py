#!/usr/bin/env python3
"""
Football Network Simulation Results Analyzer

This script analyzes the results from the NS-3 football network simulation
and generates performance reports and visualizations.
"""

import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import argparse
import os
from typing import Dict, List, Tuple

class NetworkSimulationAnalyzer:
    def __init__(self):
        self.flows = {}
        self.node_stats = {}
        self.footballer_names = {
            0: "Messi",
            1: "Ronaldo", 
            2: "Neymar",
            3: "Mbappe",
            4: "Haaland"
        }
    
    def parse_flowmon_results(self, flowmon_file: str) -> bool:
        """Parse FlowMonitor XML results"""
        try:
            tree = ET.parse(flowmon_file)
            root = tree.getroot()
            
            # Parse flow statistics
            for flow in root.findall('.//Flow'):
                flow_id = int(flow.get('flowId'))
                
                # Extract flow attributes
                tx_packets = int(flow.get('txPackets', 0))
                rx_packets = int(flow.get('rxPackets', 0))
                tx_bytes = int(flow.get('txBytes', 0))
                rx_bytes = int(flow.get('rxBytes', 0))
                
                # Calculate metrics
                packet_loss_rate = (tx_packets - rx_packets) / tx_packets if tx_packets > 0 else 0
                
                # Parse timing information
                delay_sum = float(flow.get('delaySum', 0)) / 1e9  # Convert from ns to seconds
                avg_delay = delay_sum / rx_packets if rx_packets > 0 else 0
                
                # Parse jitter
                jitter_sum = float(flow.get('jitterSum', 0)) / 1e9  # Convert from ns to seconds
                avg_jitter = jitter_sum / (rx_packets - 1) if rx_packets > 1 else 0
                
                self.flows[flow_id] = {
                    'tx_packets': tx_packets,
                    'rx_packets': rx_packets,
                    'tx_bytes': tx_bytes,
                    'rx_bytes': rx_bytes,
                    'packet_loss_rate': packet_loss_rate,
                    'avg_delay': avg_delay,
                    'avg_jitter': avg_jitter
                }
            
            print(f"Successfully parsed {len(self.flows)} flows from {flowmon_file}")
            return True
            
        except Exception as e:
            print(f"Error parsing FlowMonitor results: {e}")
            return False
    
    def calculate_throughput(self, flow_data: dict, simulation_time: float) -> float:
        """Calculate throughput in Mbps"""
        if simulation_time <= 0:
            return 0
        return (flow_data['rx_bytes'] * 8) / (simulation_time * 1e6)
    
    def generate_performance_report(self, simulation_time: float = 10.0, 
                                  image_size: int = 50000) -> str:
        """Generate a comprehensive performance report"""
        
        report = []
        report.append("=" * 60)
        report.append("FOOTBALL NETWORK SIMULATION - PERFORMANCE REPORT")
        report.append("=" * 60)
        report.append("")
        
        # Simulation parameters
        report.append("SIMULATION PARAMETERS:")
        report.append(f"  Simulation Time: {simulation_time} seconds")
        report.append(f"  Image Size: {image_size} bytes")
        report.append(f"  Number of Flows: {len(self.flows)}")
        report.append("")
        
        # Overall statistics
        total_packets_sent = sum(flow['tx_packets'] for flow in self.flows.values())
        total_packets_received = sum(flow['rx_packets'] for flow in self.flows.values())
        total_bytes_sent = sum(flow['tx_bytes'] for flow in self.flows.values())
        total_bytes_received = sum(flow['rx_bytes'] for flow in self.flows.values())
        
        overall_packet_loss = (total_packets_sent - total_packets_received) / total_packets_sent if total_packets_sent > 0 else 0
        overall_throughput = (total_bytes_received * 8) / (simulation_time * 1e6)  # Mbps
        
        report.append("OVERALL NETWORK PERFORMANCE:")
        report.append(f"  Total Packets Sent: {total_packets_sent}")
        report.append(f"  Total Packets Received: {total_packets_received}")
        report.append(f"  Overall Packet Loss Rate: {overall_packet_loss:.2%}")
        report.append(f"  Overall Throughput: {overall_throughput:.2f} Mbps")
        report.append("")
        
        # Per-flow statistics
        report.append("PER-FOOTBALLER TRANSMISSION STATISTICS:")
        report.append("-" * 60)
        
        for i, (flow_id, flow_data) in enumerate(self.flows.items()):
            footballer = self.footballer_names.get(i, f"Player_{i}")
            throughput = self.calculate_throughput(flow_data, simulation_time)
            
            report.append(f"Flow {flow_id} - {footballer}:")
            report.append(f"  Packets: {flow_data['tx_packets']} sent, {flow_data['rx_packets']} received")
            report.append(f"  Bytes: {flow_data['tx_bytes']} sent, {flow_data['rx_bytes']} received")
            report.append(f"  Packet Loss Rate: {flow_data['packet_loss_rate']:.2%}")
            report.append(f"  Average Delay: {flow_data['avg_delay']:.4f} seconds")
            report.append(f"  Average Jitter: {flow_data['avg_jitter']:.4f} seconds")
            report.append(f"  Throughput: {throughput:.2f} Mbps")
            
            # Calculate delivery efficiency
            expected_packets = (image_size + 1023) // 1024  # Assuming 1024-byte packets
            delivery_efficiency = flow_data['rx_packets'] / expected_packets if expected_packets > 0 else 0
            report.append(f"  Delivery Efficiency: {delivery_efficiency:.2%}")
            report.append("")
        
        return "\n".join(report)
    
    def create_visualizations(self, output_dir: str = "results"):
        """Create performance visualization plots"""
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Prepare data for plotting
        footballers = [self.footballer_names.get(i, f"Player_{i}") for i in range(len(self.flows))]
        packet_loss_rates = [flow['packet_loss_rate'] * 100 for flow in self.flows.values()]
        delays = [flow['avg_delay'] * 1000 for flow in self.flows.values()]  # Convert to ms
        throughputs = [self.calculate_throughput(flow, 10.0) for flow in self.flows.values()]
        
        # Create a figure with subplots
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 12))
        fig.suptitle('Football Network Simulation - Performance Analysis', fontsize=16, fontweight='bold')
        
        # Plot 1: Packet Loss Rate
        bars1 = ax1.bar(footballers, packet_loss_rates, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7'])
        ax1.set_title('Packet Loss Rate by Footballer')
        ax1.set_ylabel('Packet Loss Rate (%)')
        ax1.set_ylim(0, max(packet_loss_rates) * 1.2 if packet_loss_rates else 1)
        ax1.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars1, packet_loss_rates):
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height + 0.01,
                    f'{value:.2f}%', ha='center', va='bottom')
        
        # Plot 2: Average Delay
        bars2 = ax2.bar(footballers, delays, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7'])
        ax2.set_title('Average End-to-End Delay by Footballer')
        ax2.set_ylabel('Average Delay (ms)')
        ax2.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars2, delays):
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height + max(delays) * 0.01,
                    f'{value:.2f}', ha='center', va='bottom')
        
        # Plot 3: Throughput
        bars3 = ax3.bar(footballers, throughputs, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7'])
        ax3.set_title('Throughput by Footballer')
        ax3.set_ylabel('Throughput (Mbps)')
        ax3.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars3, throughputs):
            height = bar.get_height()
            ax3.text(bar.get_x() + bar.get_width()/2., height + max(throughputs) * 0.01,
                    f'{value:.2f}', ha='center', va='bottom')
        
        # Plot 4: Delivery Success Rate
        success_rates = [(1 - flow['packet_loss_rate']) * 100 for flow in self.flows.values()]
        bars4 = ax4.bar(footballers, success_rates, color=['#FF6B6B', '#4ECDC4', '#45B7D1', '#96CEB4', '#FFEAA7'])
        ax4.set_title('Delivery Success Rate by Footballer')
        ax4.set_ylabel('Success Rate (%)')
        ax4.set_ylim(0, 100)
        ax4.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, value in zip(bars4, success_rates):
            height = bar.get_height()
            ax4.text(bar.get_x() + bar.get_width()/2., height + 1,
                    f'{value:.1f}%', ha='center', va='bottom')
        
        plt.tight_layout()
        plt.savefig(f"{output_dir}/performance_analysis.png", dpi=300, bbox_inches='tight')
        plt.savefig(f"{output_dir}/performance_analysis.pdf", bbox_inches='tight')
        print(f"Performance plots saved to {output_dir}/")
        
        # Create a summary comparison plot
        fig2, ax = plt.subplots(1, 1, figsize=(12, 8))
        
        x = np.arange(len(footballers))
        width = 0.25
        
        # Normalize metrics for comparison (0-100 scale)
        norm_throughput = [t/max(throughputs)*100 if throughputs else 0 for t in throughputs]
        norm_success = success_rates
        norm_delay = [100 - (d/max(delays)*100) if delays else 0 for d in delays]  # Invert delay (lower is better)
        
        bars1 = ax.bar(x - width, norm_throughput, width, label='Throughput', color='#4ECDC4')
        bars2 = ax.bar(x, norm_success, width, label='Success Rate', color='#96CEB4')
        bars3 = ax.bar(x + width, norm_delay, width, label='Delay Performance', color='#FFEAA7')
        
        ax.set_title('Normalized Performance Comparison (0-100 scale)')
        ax.set_ylabel('Normalized Score')
        ax.set_xlabel('Footballer')
        ax.set_xticks(x)
        ax.set_xticklabels(footballers)
        ax.legend()
        ax.set_ylim(0, 100)
        
        plt.xticks(rotation=45)
        plt.tight_layout()
        plt.savefig(f"{output_dir}/performance_comparison.png", dpi=300, bbox_inches='tight')
        plt.savefig(f"{output_dir}/performance_comparison.pdf", bbox_inches='tight')
        
        plt.show()
    
    def export_csv_report(self, output_dir: str = "results", simulation_time: float = 10.0):
        """Export detailed results to CSV"""
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Prepare data for CSV
        data = []
        for i, (flow_id, flow_data) in enumerate(self.flows.items()):
            footballer = self.footballer_names.get(i, f"Player_{i}")
            throughput = self.calculate_throughput(flow_data, simulation_time)
            
            data.append({
                'Flow_ID': flow_id,
                'Footballer': footballer,
                'Packets_Sent': flow_data['tx_packets'],
                'Packets_Received': flow_data['rx_packets'],
                'Bytes_Sent': flow_data['tx_bytes'],
                'Bytes_Received': flow_data['rx_bytes'],
                'Packet_Loss_Rate_%': flow_data['packet_loss_rate'] * 100,
                'Avg_Delay_ms': flow_data['avg_delay'] * 1000,
                'Avg_Jitter_ms': flow_data['avg_jitter'] * 1000,
                'Throughput_Mbps': throughput,
                'Success_Rate_%': (1 - flow_data['packet_loss_rate']) * 100
            })
        
        df = pd.DataFrame(data)
        csv_file = f"{output_dir}/simulation_results.csv"
        df.to_csv(csv_file, index=False)
        print(f"Detailed results exported to {csv_file}")
        
        return df

def main():
    parser = argparse.ArgumentParser(description='Analyze Football Network Simulation Results')
    parser.add_argument('--flowmon', default='flowmon-results.xml', 
                       help='FlowMonitor XML results file')
    parser.add_argument('--output', default='results', 
                       help='Output directory for reports and plots')
    parser.add_argument('--simulation-time', type=float, default=10.0,
                       help='Simulation time in seconds')
    parser.add_argument('--image-size', type=int, default=50000,
                       help='Image size in bytes')
    parser.add_argument('--no-plots', action='store_true',
                       help='Skip generating plots')
    
    args = parser.parse_args()
    
    # Create analyzer instance
    analyzer = NetworkSimulationAnalyzer()
    
    # Check if FlowMonitor file exists
    if not os.path.exists(args.flowmon):
        print(f"Error: FlowMonitor file '{args.flowmon}' not found.")
        print("Please run the NS-3 simulation first to generate results.")
        return 1
    
    # Parse results
    if not analyzer.parse_flowmon_results(args.flowmon):
        return 1
    
    # Generate performance report
    report = analyzer.generate_performance_report(args.simulation_time, args.image_size)
    print(report)
    
    # Save report to file
    if not os.path.exists(args.output):
        os.makedirs(args.output)
    
    report_file = f"{args.output}/performance_report.txt"
    with open(report_file, 'w') as f:
        f.write(report)
    print(f"\nPerformance report saved to {report_file}")
    
    # Export CSV
    analyzer.export_csv_report(args.output, args.simulation_time)
    
    # Generate visualizations
    if not args.no_plots:
        try:
            analyzer.create_visualizations(args.output)
        except ImportError:
            print("Warning: matplotlib not available. Skipping plot generation.")
            print("Install matplotlib with: pip install matplotlib")
    
    return 0

if __name__ == "__main__":
    exit(main())
