

python python/recoShowers.py --source /data_linux/dedx_files/nu_electrons_bootleg.root --data-output /data_linux/dedx_files/nu_electrons_reconstructed.root
python python/recoShowers.py --source /data_linux/dedx_files/anu_electrons_bootleg.root --data-output /data_linux/dedx_files/anu_electrons_reconstructed.root


python python/recoShowers.py --source /data_linux/dedx_files/nu_photons_bootleg.root --data-output /data_linux/dedx_files/nu_photons_reconstructed.root
python python/recoShowers.py --source /data_linux/dedx_files/anu_photons_bootleg.root --data-output /data_linux/dedx_files/anu_photons_reconstructed.root

python analysis/dEdxPlotter.py