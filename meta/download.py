import zipfile
import os
import urllib.request
from tqdm import tqdm


class DownloadProgressBar(tqdm):
    def update_to(self, b=1, bsize=1, tsize=None):
        if tsize is not None:
            self.total = tsize
        self.update(b * bsize - self.n)


def download_url(url, name, ext, folder: str = "."):
    fpath = os.path.join(folder, f"{name}{ext}")
    with DownloadProgressBar(
        unit="B", unit_scale=True, miniters=1, desc=url.split("/")[-1]
    ) as t:
        urllib.request.urlretrieve(url, filename=fpath, reporthook=t.update_to)
    return fpath


if __name__ == "__main__":
    urls = {i: f"https://dd.b.pvp.net/latest/set{i}-en_us.zip" for i in range(1, 4)}
    folder = "./sets"
    if not os.path.isdir(folder):
        os.mkdir(folder)

    for i, url in urls.items():
        fpath = download_url(url, f"set{i}-en_us", ".zip", folder)
        with zipfile.ZipFile(fpath, "r") as zip_ref:
            zip_ref.extractall(os.path.splitext(fpath)[0])
        os.remove(fpath)
