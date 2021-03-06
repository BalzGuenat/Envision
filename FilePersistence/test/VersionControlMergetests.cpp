/***********************************************************************************************************************
**
** Copyright (c) 2011, 2015 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#include "FilePersistencePlugin.h"
#include "SelfTest/src/SelfTestSuite.h"

#include "version_control/Diff.h"
#include "version_control/Merge.h"
#include "version_control/GitRepository.h"


namespace FilePersistence {

static void unpack(QString testName)
{
	auto command = "tar -xf TestMerge_" + testName + ".tar -C projects";
	Q_ASSERT(std::system(command.toStdString().c_str()) == 0);
}

static void cleanup()
{
	Q_ASSERT(std::system("rm -r projects/TestMerge") == 0);
}

TEST(FilePersistencePlugin, TwoDeletesNoConflict)
{
	unpack(this->getName());
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	auto tree = merge->mergedTree();
	merge->commit(sig, sig, "This is the result of merge test \"twodeletesNoConflict\"");
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000000032}")));
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000000042}")));
	cleanup();
}

TEST(FilePersistencePlugin, TwoDeletesInSameListResolvable)
{
	unpack(this->getName());
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	auto tree = merge->mergedTree();
	merge->commit(sig, sig, "This is the result of merge test \"" + this->getName() + "\"");
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000000013}")));
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000000042}")));
	// check that the list is continuous.
	auto listContainer = tree->find(QUuid("{00000000-0000-0000-0000-000000000002}"));
	for (int idx = 0; idx < listContainer->children().size(); ++idx)
		CHECK_CONDITION(listContainer->child(QString::number(idx)));
	cleanup();
}

TEST(FilePersistencePlugin, UnorderedAndUnitsConflicting)
{
	unpack(this->getName());
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	CHECK_CONDITION(merge->hasConflicts());
	QSet<Model::NodeIdType> expected = {
		QUuid("{00000000-0000-0000-0000-000000000047}"),
		QUuid("{00000000-0000-0000-0000-000000000048}"),
		QUuid("{00000000-0000-0000-0000-000000000031}"),
		QUuid("{00000000-0000-0000-0000-000000000030}"),
		QUuid("{00000000-0000-0000-0000-000000000051}"),
		QUuid("{00000000-0000-0000-0000-000000000052}")
	};
	for (auto change : merge->getConflicts())
	{
		CHECK_CONDITION(expected.contains(change->nodeId()));
		expected.remove(change->nodeId());
	}
	CHECK_CONDITION(expected.isEmpty());

	cleanup();
}

TEST(FilePersistencePlugin, ListsReorderInsertDeleteResolvable)
{
	unpack(this->getName());
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "This is the result of merge test \"" + this->getName() + "\"");

	auto tree = merge->mergedTree();
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000021404}")));
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000212}")));
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000000506}")));
	CHECK_CONDITION(!tree->find(QUuid("{00000000-0000-0000-0000-000000001507}")));

	auto listContainer = tree->find(QUuid("{00000000-0000-0000-0000-000000000200}"));
	for (int idx = 0; idx < listContainer->children().size(); ++idx)
		CHECK_CONDITION(listContainer->child(QString::number(idx)));

	CHECK_CONDITION(listContainer->child(QString::number(1))->id().toString().endsWith("207}"));
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000203}"))->label().toInt() == 6);
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000204}"))->label().toInt() == 12);
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000205}"))->label().toInt() == 10);
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000206}"))->label().toInt() == 11);
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000207}"))->label().toInt() == 1);
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000000211}"))
						 ->parent()->id().toString().endsWith("300}"));
	CHECK_CONDITION(tree->find(QUuid("{00000000-0000-0000-0000-000000011507}"))
						 ->parent()->id().toString().endsWith("507}"));
	cleanup();
}

TEST(FilePersistencePlugin, EvalClassMove)
{
	QString command = "tar -xf Eval_ClassMove.tar -C projects";
	Q_ASSERT(std::system(command.toStdString().c_str()) == 0);
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "Merged master and dev");
	cleanup();
}

TEST(FilePersistencePlugin, EvalLoopFix)
{
	QString command = "tar -xf Eval_LoopFix.tar -C projects";
	Q_ASSERT(std::system(command.toStdString().c_str()) == 0);
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	CHECK_CONDITION(merge->hasConflicts());
	cleanup();
}

TEST(FilePersistencePlugin, EvalMethodInsert)
{
	QString command = "tar -xf Eval_MethodInsert.tar -C projects";
	Q_ASSERT(std::system(command.toStdString().c_str()) == 0);
	GitRepository repo("projects/TestMerge");
	auto merge = repo.merge("dev");
	CHECK_CONDITION(!merge->hasConflicts());
	Signature sig;
	sig.name_ = "Chuck TESTa";
	sig.eMail_ = "chuck@mergetest.com";
	merge->commit(sig, sig, "Merged master and dev");
	cleanup();
}

} /* namespace FilePersistence */
